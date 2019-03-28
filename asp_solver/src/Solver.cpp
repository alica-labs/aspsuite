#include "reasoner/asp/Solver.h"

#include "reasoner/asp/AnnotatedExternal.h"
#include "reasoner/asp/ExtensionQuery.h"
#include "reasoner/asp/FilterQuery.h"

#include <reasoner/asp/AnnotatedValVec.h>
#include <reasoner/asp/Query.h>
#include <reasoner/asp/Term.h>
#include <reasoner/asp/Variable.h>

#define Solver_DEBUG

namespace reasoner
{
namespace asp
{

const void* const Solver::WILDCARD_POINTER = new int(0);
const std::string Solver::WILDCARD_STRING = "wildcard";

std::mutex Solver::queryCounterMutex;

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
    this->clingo = std::make_shared<Clingo::Control>(args, logger, 20);
    this->clingo->register_observer(this->observer);
    this->sc = essentials::SystemConfig::getInstance();
    this->queryCounter = 0;
    // should make the solver return all models (because you set it to 0)
    this->clingo->configuration()["solve"]["models"] = "0";
}

Solver::~Solver()
{
}

void Solver::loadFile(std::string absolutFilename)
{
    this->clingo->load(absolutFilename.c_str());
}

bool Solver::loadFileFromConfig(std::string configKey)
{
    if (configKey.empty()) {
        return true;
    }

    for (auto file : this->alreadyLoaded) {
        if (configKey.compare(file) == 0) {
            return false;
        }
    }

    std::string backGroundKnowledgeFile = (*this->sc)["Solver"]->get<std::string>(configKey.c_str(), NULL);
    this->alreadyLoaded.push_back(configKey.c_str());
    backGroundKnowledgeFile = essentials::FileSystem::combinePaths((*this->sc).getConfigPath(), backGroundKnowledgeFile);
    this->clingo->load(backGroundKnowledgeFile.c_str());
    return true;
}

/**
 * Let the internal solver ground a given program part (context).
 */
void Solver::ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack)
{
#ifdef ASPSOLVER_DEBUG
    cout << "Solver_ground: " << vec.at(0).first << endl;
#endif
    this->observer.clear();
    this->clingo->ground(vec, callBack);
}

/**
 * Let the internal solver solve one time.
 */
bool Solver::solve()
{
    this->currentModels.clear();
    this->reduceQueryLifeTime();
    // bind(&Solver::onModel, this, placeholders::_1)
    Clingo::SymbolicLiteralSpan span = {};
    auto result = this->clingo->solve(span, this, false, false);
    return result.get().is_satisfiable();
}

/**
 * Callback for created models during solving.
 */
bool Solver::on_model(Clingo::Model& m)
{
#ifdef Solver_DEBUG
    std::cout << "Solver: Found the following model :" << std::endl;
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
        query->onModel(m);
    }
    return true;
}

void Solver::assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue)
{
    this->clingo->assign_external(ext, truthValue);
}

void Solver::releaseExternal(Clingo::Symbol ext)
{
    this->clingo->release_external(ext);
}

void Solver::add(char const* name, const Clingo::StringSpan& params, char const* par)
{
    this->clingo->add(name, params, par);
}

Clingo::Symbol Solver::parseValue(const std::string& str)
{
    return Clingo::parse_term(str.c_str(), nullptr, 20);
}

bool Solver::registerQuery(std::shared_ptr<Query> query)
{
    auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
    if (entry == this->registeredQueries.end()) {
        this->registeredQueries.push_back(query);
        return true;
    }
    return false;
}

bool Solver::unregisterQuery(std::shared_ptr<Query> query)
{
    query->removeExternal();
    auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
    if (entry != this->registeredQueries.end()) {
        this->registeredQueries.erase(entry);
        return true;
    }
    return false;
}

bool Solver::existsSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls)
{
    this->clingo->configuration()["solve"]["models"] = "1";
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
    this->clingo->configuration()["solve"]["models"] = "0";
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
    for (auto& queriedId : this->currentQueryIds) {
        for (auto& query : this->registeredQueries) {
            std::vector<Clingo::SymbolVector> vals;
            for (auto& pair : query->getHeadValues()) {
                vals.push_back(pair.second);
            }
            if (queriedId == query->getTerm()->getId()) {
                results.push_back(new AnnotatedValVec(query->getTerm()->getId(), vals, query));
                break;
            }
        }
    }
    this->removeDeadQueries();
    this->currentQueryIds.clear();
    if (results.size() > 0) {
        return true;
    } else {
        return false;
    }
}

int Solver::prepareSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls)
{
    auto cVars = std::vector<Variable*>(vars.size());
    for (int i = 0; i < vars.size(); ++i) {
        cVars.at(i) = vars.at(i);
    }
    for (auto term : calls) {
        this->currentQueryIds.push_back(term->getId());
        if (!term->getNumberOfModels().empty()) {
            this->clingo->configuration()["solve"]["models"] = term->getNumberOfModels().c_str();
        }
        bool found = false;
        for (auto query : this->registeredQueries) {
            if (term->getId() == query->getTerm()->getId()) {
                found = true;
            }
        }
        if (!found) {
            if (term->getType() == QueryType::Extension) {
                this->registerQuery(std::make_shared<ExtensionQuery>(this, term));
            } else if (term->getType() == QueryType::Filter) {
                this->registerQuery(std::make_shared<FilterQuery>(this, term));
            } else {
                std::cout << "Solver: Query of unknown type registered!" << std::endl;
                return -1;
            }
        }
    }

    // Handle externals from registered queries and the term from this solve call
    for (auto q : this->registeredQueries) {
        auto ext = q->getTerm()->getExternals();
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
    for (int i = this->registeredQueries.size() - 1; i >= 0; i--) {
        if (this->registeredQueries.at(i)->getLifeTime() == 0) {
            this->unregisterQuery(this->registeredQueries.at(i));
        }
    }
}

const void* Solver::getWildcardPointer()
{
    return WILDCARD_POINTER;
}

const std::string& Solver::getWildcardString()
{
    return WILDCARD_STRING;
}

int Solver::getRegisteredQueriesCount()
{
    return this->registeredQueries.size();
}

void Solver::reduceQueryLifeTime()
{
    for (auto query : this->registeredQueries) {
        query->reduceLifeTime();
    }
}

const double Solver::getSolvingTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["solve"] * 1000;
}

const double Solver::getSatTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["sat"] * 1000;
}

const double Solver::getUnsatTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["unsat"] * 1000;
}

const double Solver::getModelCount()
{
    return this->clingo->statistics()["summary"]["models"]["enumerated"];
}

int Solver::getQueryCounter()
{
    std::lock_guard<std::mutex> lock(queryCounterMutex);

    this->queryCounter++;
    return this->queryCounter;
}

const double Solver::getAtomCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms"];
}

const double Solver::getBodiesCount()
{
    return this->clingo->statistics()["problem"]["lp"]["bodies"];
}

const double Solver::getAuxAtomsCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms_aux"];
}

std::vector<Clingo::SymbolVector> Solver::getCurrentModels()
{
    return this->currentModels;
}

void Solver::printStats()
{
    auto statistics = this->clingo->statistics();

    std::stringstream ss;
    ss << "Solve Statistics:" << std::endl;
    ss << "TOTAL Time: " << statistics["sumary"]["times"]["total"] << "s" << std::endl;
    ss << "CPU Time: " << statistics["sumary"]["times"]["cpu"] << "s" << std::endl;
    ss << "SAT Time: " << (statistics["sumary"]["times"]["sat"] * 1000.0) << "ms" << std::endl;
    ss << "UNSAT Time: " << (statistics["sumary"]["times"]["unsat"] * 1000.0) << "ms" << std::endl;
    ss << "SOLVE Time: " << (statistics["sumary"]["times"]["solve"] * 1000.0) << "ms" << std::endl;
    std::cout << ss.str() << std::flush;
}

std::vector<std::shared_ptr<Query>> Solver::getRegisteredQueries()
{
    return registeredQueries;
}

const std::string Solver::getGroundProgram() const
{
    return this->observer.getGroundProgram();
}

} /* namespace asp */
} /* namespace reasoner */
