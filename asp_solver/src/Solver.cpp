#include "reasoner/asp/Solver.h"

#include "reasoner/asp/AnnotatedExternal.h"
#include "reasoner/asp/AnnotatedValVec.h"
#include "reasoner/asp/ExtensionQuery.h"
#include "reasoner/asp/FilterQuery.h"
#include "reasoner/asp/IncrementalExtensionQuery.h"
#include "reasoner/asp/Query.h"
#include "reasoner/asp/ReusableExtensionQuery.h"
#include "reasoner/asp/Term.h"
#include "reasoner/asp/Variable.h"

#include <chrono>
#include <utility>

#define SOLVER_DEBUG

namespace reasoner
{
namespace asp
{

const void* const Solver::WILDCARD_POINTER = new int(0);
const std::string Solver::WILDCARD_STRING = "wildcard";

std::mutex Solver::queryCounterMutex;
std::mutex Solver::clingoMtx;

Solver::Solver(std::vector<char const*> args)
{
    Clingo::Logger logger = [](Clingo::WarningCode warningCode, char const* message) {
        switch (warningCode) {
        case Clingo::WarningCode::AtomUndefined:
        case Clingo::WarningCode::FileIncluded:
        case Clingo::WarningCode::GlobalVariable:
        case Clingo::WarningCode::VariableUnbounded:
        case Clingo::WarningCode::Other:
        case Clingo::WarningCode::OperationUndefined:
        case Clingo::WarningCode::RuntimeError:
            break;
        default:
            std::cerr << message << std::endl;
        }
    };
    this->clingo = new Clingo::Control(args, logger, 20);
    this->clingo->register_observer(this->observer);
    this->sc = essentials::SystemConfig::getInstance();
    this->queryCounter = 0;
    this->clingo->configuration()["configuration"] = "handy";
    // should make the solver return all models (because you set it to 0)
    this->clingo->configuration()["solve"]["models"] = "0";
}

Solver::~Solver() = default;

void Solver::loadFile(const std::string& absolutFilename)
{
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->clingo->load(absolutFilename.c_str());
}

bool Solver::loadFileFromConfig(const std::string& configKey)
{
    if (configKey.empty()) {
        return true;
    }

    for (const auto& file : this->alreadyLoaded) {
        if (configKey.compare(file) == 0) {
            return false;
        }
    }

    std::string backGroundKnowledgeFile = (*this->sc)["Solver"]->get<std::string>(configKey.c_str(), NULL);
    this->alreadyLoaded.push_back(configKey.c_str());
    backGroundKnowledgeFile = essentials::FileSystem::combinePaths((*this->sc).getConfigPath(), backGroundKnowledgeFile);
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->clingo->load(backGroundKnowledgeFile.c_str());
    return true;
}

/**
 * Let the internal solver ground a given program part (context).
 */
void Solver::ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack)
{
#ifdef SOLVER_DEBUG
    std::cout << "[Solver] grounding ";
    for (auto& part : vec ) {
        std::cout << "'" << part.name() << "(";
        for (auto& param : part.params()) {
            std::cout << param.to_string();
        }
        std::cout << ")' ";
    }
    std::cout << std::endl;
#endif
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->observer.clear();
    this->clingo->ground(vec, std::move(callBack));
}

/**
 * Let the internal solver solve one time.
 */
bool Solver::solve()
{
#ifdef SOLVER_DEBUG
        std::cout << "[Solver] solving " << std::endl;
#endif
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->currentModels.clear();
    this->reduceQueryLifeTime();
    Clingo::SymbolicLiteralSpan span = {};
    auto result = this->clingo->solve(span, this, false, false);
    return result.get().is_satisfiable();
}

/**
 * Callback for created models during solving.
 *
 * ATTENTION: Must exactly be named 'on_model', because clingo expects that.
 */
bool Solver::on_model(Clingo::Model& m)
{
#ifdef SOLVER_DEBUG
    std::cout << "[Solver] Found the following model: " << std::endl;
    for (auto& atom : m.symbols(Clingo::ShowType::Shown)) {
        std::cout << atom << " ";
    }
    std::cout << std::endl;
#endif
    Clingo::SymbolVector vec;
    auto tmp = m.symbols(Clingo::ShowType::Shown);
    for (int i = 0; i < tmp.size(); i++) {
        vec.push_back(tmp[i]);
    }
    this->currentModels.push_back(vec);
    for (auto& query : this->registeredQueries) {
        query.second->onModel(m);
    }
    return true;
}

void Solver::assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue)
{
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->clingo->assign_external(ext, truthValue);
}

void Solver::releaseExternal(Clingo::Symbol ext)
{
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->clingo->release_external(ext);
}

void Solver::add(char const* name, const Clingo::StringSpan& params, char const* par)
{
#ifdef SOLVER_DEBUG
    std::cout << "[Solver] adding '" << par << "'" << std::endl;
#endif
    std::lock_guard<std::mutex> lock(clingoMtx);
    this->clingo->add(name, params, par);
}

Clingo::Symbol Solver::parseValue(const std::string& str)
{
    return Clingo::parse_term(str.c_str(), nullptr, 20);
}

void Solver::registerQuery(std::shared_ptr<Query> query)
{
    auto entry = this->registeredQueries.find(query->getQueryID());
    if (entry == this->registeredQueries.end()) {
        this->registeredQueries.emplace(query->getQueryID(), query);
    }
}

void Solver::unregisterQuery(int queryID)
{
    auto entry = this->registeredQueries.find(queryID);
    entry->second->removeExternal();
    this->registeredQueries.erase(entry->first);
}

bool Solver::existsSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls)
{
    this->clingo->configuration()["solve"]["models"] = "1"; // get only one model
    int dim = prepareSolution(vars, calls);
    if (dim == -1) {
        return false;
    }
    auto satisfied = this->solve();
    this->removeDeadQueries();
    return satisfied;
}

bool Solver::getSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls, std::vector<AnnotatedValVec*>& results)
{
    this->clingo->configuration()["solve"]["models"] = "0"; // get all models
    int dim = prepareSolution(vars, calls);
    if (dim == -1) {
        this->removeDeadQueries();
        return false;
    }
    auto satisfied = this->solve();
    if (!satisfied) {
        this->removeDeadQueries();
        return false;
    }

    for (auto& queryEntry : this->registeredQueries) {
        std::vector<Clingo::SymbolVector> vals;
        // FIXME: only returns values from first model (idx = 0)
        auto& mapping = queryEntry.second->getQueryResultMappings()[0];
        for (auto& pair : mapping) {
            vals.push_back(pair.second);
        }
        results.push_back(new AnnotatedValVec(queryEntry.second->getTerm()->getId(), vals, queryEntry.second));
    }

    this->removeDeadQueries();
    if (results.size() > 0) {
        return true;
    } else {
        return false;
    }
}

int Solver::prepareSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls)
{
    for (auto term : calls) {
        int queryID = this->generateQueryID();
        if (!term->getNumberOfModels().empty()) {
            this->clingo->configuration()["solve"]["models"] = term->getNumberOfModels().c_str();
        }
        bool found = false;
        for (auto& query : this->registeredQueries) {
            if (queryID == query.second->getQueryID()) {
                found = true;
            }
        }
        if (!found) {
            if (term->getType() == QueryType::Extension) {
                this->registerQuery(std::make_shared<ExtensionQuery>(queryID, this, term));
            } else if (term->getType() == QueryType::Filter) {
                this->registerQuery(std::make_shared<FilterQuery>(queryID, this, term));
            } else if (term->getType() == QueryType::IncrementalExtension) {
                this->registerQuery(std::make_shared<IncrementalExtensionQuery>(queryID, this, term));
            } else if (term->getType() == QueryType::ReusableExtension) {
                this->registerQuery(std::make_shared<ReusableExtensionQuery>(queryID, this, term));
            } else {
                std::cout << "[Solver] Query of unknown type registered!" << std::endl;
                return -1;
            }
        }
    }

    // Handle externals from registered queries and the term from this solve call
    for (auto& q : this->registeredQueries) {
        auto ext = q.second->getTerm()->getExternals();
        if (ext != nullptr) {
            this->handleExternals(ext);
        }
    }
    return vars.size();
}

void Solver::handleExternals(std::shared_ptr<std::map<std::string, bool>> externals)
{
    for (auto p : *externals) {
        auto it = find_if(this->assignedExternals.begin(), this->assignedExternals.end(),
                [p](std::shared_ptr<AnnotatedExternal> element) { return element->getAspPredicate() == p.first; });
        if (it == this->assignedExternals.end()) {
            std::shared_ptr<Clingo::Symbol> val = std::make_shared<Clingo::Symbol>(Clingo::parse_term(p.first.c_str()));
            this->clingo->assign_external(*val, p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
            this->assignedExternals.push_back(std::make_shared<AnnotatedExternal>(p.first, val, p.second));
        } else {
            if (p.second != (*it)->getValue()) {
                this->clingo->assign_external(*((*it)->getGringoValue()), p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
                (*it)->setValue(p.second);
            }
        }
    }
}

std::shared_ptr<Variable> Solver::createVariable(long id)
{
    return std::make_shared<Variable>();
}

void Solver::removeDeadQueries()
{
    for (auto it = this->registeredQueries.begin(); it != this->registeredQueries.end();) {
        if (it->second->getLifeTime() == 0) {
            this->unregisterQuery(it->first);
        } else {
            it++;
        }
    }
}

void Solver::reduceQueryLifeTime()
{
    for (auto& queryEntry : this->registeredQueries) {
        queryEntry.second->reduceLifeTime();
    }
}

double Solver::getSolvingTime()
{
    return this->clingo->statistics()["sumary"]["times"]["solve"] * 1000;
}

double Solver::getSatTime()
{
    return this->clingo->statistics()["sumary"]["times"]["sat"] * 1000;
}

double Solver::getUnsatTime()
{
    return this->clingo->statistics()["sumary"]["times"]["unsat"] * 1000;
}

double Solver::getModelCount()
{
    return this->clingo->statistics()["summary"]["models"]["enumerated"];
}

void Solver::setNumberOfModels(int numberOfModels)
{
    this->clingo->configuration()["solve"]["models"] = std::to_string(numberOfModels).c_str();
}

int Solver::generateQueryID()
{
    std::lock_guard<std::mutex> lock(queryCounterMutex);
    return ++this->queryCounter;
}

double Solver::getAtomCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms"];
}

double Solver::getBodiesCount()
{
    return this->clingo->statistics()["problem"]["lp"]["bodies"];
}

double Solver::getAuxAtomsCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms_aux"];
}

std::vector<Clingo::SymbolVector> Solver::getCurrentModels()
{
    return this->currentModels;
}

Clingo::SymbolicAtoms Solver::getSymbolicAtoms()
{
    return this->clingo->symbolic_atoms();
}

void Solver::printStats()
{
    auto statistics = this->clingo->statistics();

    std::stringstream ss;
    ss << "Solve Statistics:" << std::endl;
    ss << "TOTAL Time: " << statistics["summary"]["times"]["total"] << "s" << std::endl;
    ss << "CPU Time: " << statistics["summary"]["times"]["cpu"] << "s" << std::endl;
    ss << "SAT Time: " << (statistics["summary"]["times"]["sat"] * 1000.0) << "ms" << std::endl;
    ss << "UNSAT Time: " << (statistics["summary"]["times"]["unsat"] * 1000.0) << "ms" << std::endl;
    ss << "SOLVE Time: " << (statistics["summary"]["times"]["solve"] * 1000.0) << "ms" << std::endl;
    std::cout << ss.str() << std::flush;
}

const std::unordered_map<int, std::shared_ptr<Query>> Solver::getRegisteredQueries() const
{
    return registeredQueries;
}

const std::string Solver::getGroundProgram() const
{
    return this->observer.getGroundProgram();
}

} /* namespace asp */
} /* namespace reasoner */
