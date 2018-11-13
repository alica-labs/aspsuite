#include <asp_solver/ASPSolver.h>

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/AnnotatedValVec.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_solver/ASPVariableQuery.h>
#include <asp_solver/AnnotatedExternal.h>

namespace reasoner
{

mutex ASPSolver::queryCounterMutex;

ASPSolver::ASPSolver(vector<char const*> args)
        : IASPSolver()
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
    this->clingo = make_shared<Clingo::Control>(args, logger, 20);
    this->sc = supplementary::SystemConfig::getInstance();
    this->queryCounter = 0;
#ifdef ASPSolver_DEBUG
    this->modelCount = 0;
#endif
    // should make the solver return all models (because you set it to 0)
    this->clingo->configuration()["solve"]["models"] = "0";
#ifdef SOLVER_OPTIONS
    traverseOptions(conf, root, "");
#endif
}

ASPSolver::~ASPSolver() {}

void ASPSolver::loadFile(string absolutFilename)
{
    this->clingo->load(absolutFilename.c_str());
}

bool ASPSolver::loadFileFromConfig(string configKey)
{

    if (configKey.empty()) {
        return true;
    }
    for (auto file : this->alreadyLoaded) {
        if (configKey.compare(file) == 0) {
            return false;
        }
    }

    string backGroundKnowledgeFile = (*this->sc)["ASPSolver"]->get<string>(configKey.c_str(), NULL);
    this->alreadyLoaded.push_back(configKey.c_str());
    backGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*this->sc).getConfigPath(), backGroundKnowledgeFile);
    this->clingo->load(backGroundKnowledgeFile.c_str());
    return true;
}

/**
 * Let the internal solver ground a given program part (context).
 */
void ASPSolver::ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack)
{
#ifdef ASPSOLVER_DEBUG
    cout << "ASPSolver_ground: " << vec.at(0).first << endl;
#endif
    this->clingo->ground(vec, callBack);
}

/**
 * Let the internal solver solve one time.
 */
bool ASPSolver::solve()
{
    this->currentModels.clear();
    this->reduceQueryLifeTime();
#ifdef ASPSolver_DEBUG
    this->modelCount = 0;
#endif
    // bind(&ASPSolver::onModel, this, placeholders::_1)
    Clingo::SymbolicLiteralSpan span = {};
    auto result = this->clingo->solve(span, this);
    return result.get().is_satisfiable();
}

/**
 * Callback for created models during solving.
 */
bool ASPSolver::on_model(Clingo::Model& m)
{
#ifdef ASPSolver_DEBUG
    cout << "ASPSolver: Found the following model :" << endl;
    for (auto& atom : m.symbols(Clingo::ShowType::Shown)) {
        cout << atom << " ";
    }
    cout << endl;
#endif
    Clingo::Model& clingoModel = (Clingo::Model&) m;
    Clingo::SymbolVector vec;
    auto tmp = clingoModel.symbols(clingo_show_type_shown);
    for (int i = 0; i < tmp.size(); i++) {
        vec.push_back(tmp[i]);
    }
    this->currentModels.push_back(vec);
    for (auto& query : this->registeredQueries) {
        query->onModel(clingoModel);
    }
    return true;
}

void ASPSolver::assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue)
{
    this->clingo->assign_external(ext, truthValue);
}

void ASPSolver::releaseExternal(Clingo::Symbol ext)
{
    this->clingo->release_external(ext);
}

void ASPSolver::add(char const* name, const Clingo::StringSpan& params, char const* par)
{
    this->clingo->add(name, params, par);
}

Clingo::Symbol ASPSolver::parseValue(const std::string& str)
{
    return Clingo::parse_term(str.c_str(), nullptr, 20);
}

bool ASPSolver::registerQuery(shared_ptr<ASPQuery> query)
{
    auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
    if (entry == this->registeredQueries.end()) {
        this->registeredQueries.push_back(query);
        return true;
    }
    return false;
}

bool ASPSolver::unregisterQuery(shared_ptr<ASPQuery> query)
{
    query->removeExternal();
    auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
    if (entry != this->registeredQueries.end()) {
        this->registeredQueries.erase(entry);
        return true;
    }
    return false;
}

bool ASPSolver::existsSolution(vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls)
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

bool ASPSolver::getSolution(vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls, vector<AnnotatedValVec*>& results)
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
            vector<Clingo::SymbolVector> vals;
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

int ASPSolver::prepareSolution(vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls)
{

    auto cVars = vector<reasoner::ASPCommonsVariable*>(vars.size());
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
            if (term->getType() == ASPQueryType::Variable) {
                this->registerQuery(make_shared<ASPVariableQuery>(this, term));
            }  else if (term->getType() == ASPQueryType::Facts) {
                this->registerQuery(make_shared<ASPFactsQuery>(this, term));
            } else {
                cout << "ASPSolver: Query of unknown type registered!" << endl;
                return -1;
            }
        }
        if (term->getExternals() != nullptr) {
            for (auto p : *term->getExternals()) {
                auto it = find_if(this->assignedExternals.begin(), this->assignedExternals.end(),
                        [p](shared_ptr<AnnotatedExternal> element) { return element->getAspPredicate() == p.first; });
                if (it == this->assignedExternals.end()) {
                    shared_ptr<Clingo::Symbol> val = make_shared<Clingo::Symbol>(Clingo::parse_term(p.first.c_str()));
                    this->clingo->assign_external(*val, p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
                    this->assignedExternals.push_back(make_shared<AnnotatedExternal>(p.first, val, p.second));
                } else {
                    if (p.second != (*it)->getValue()) {
                        this->clingo->assign_external(*((*it)->getGringoValue()), p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
                        (*it)->setValue(p.second);
                    }
                }
            }
        }
    }
    return vars.size();
}

shared_ptr<ASPCommonsVariable> ASPSolver::createVariable(long id)
{
    return make_shared<reasoner::ASPCommonsVariable>();
}

void ASPSolver::removeDeadQueries()
{
    for (int i = this->registeredQueries.size() - 1; i >= 0; i--) {
        if (this->registeredQueries.at(i)->getLifeTime() == 0) {
            this->unregisterQuery(this->registeredQueries.at(i));
        }
    }
}

const void* ASPSolver::getWildcardPointer()
{
    return WILDCARD_POINTER;
}

const string& ASPSolver::getWildcardString()
{
    return WILDCARD_STRING;
}

int ASPSolver::getRegisteredQueriesCount()
{
    return this->registeredQueries.size();
}

void ASPSolver::reduceQueryLifeTime()
{
    for (auto query : this->registeredQueries) {
        query->reduceLifeTime();
    }
}

const double ASPSolver::getSolvingTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["solve"] * 1000;
}

const double ASPSolver::getSatTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["sat"] * 1000;
}

const double ASPSolver::getUnsatTime()
{
    auto statistics = this->clingo->statistics();

    // time in seconds
    return statistics["sumary"]["times"]["unsat"] * 1000;
}

const double ASPSolver::getModelCount()
{
    return this->clingo->statistics()["summary"]["models"]["enumerated"];
}

int ASPSolver::getQueryCounter()
{
    lock_guard<mutex> lock(queryCounterMutex);

    this->queryCounter++;
    return this->queryCounter;
}

const double ASPSolver::getAtomCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms"];
}

const double ASPSolver::getBodiesCount()
{
    return this->clingo->statistics()["problem"]["lp"]["bodies"];
}

const double ASPSolver::getAuxAtomsCount()
{
    return this->clingo->statistics()["problem"]["lp"]["atoms_aux"];
}

vector<Clingo::SymbolVector> ASPSolver::getCurrentModels()
{
    return this->currentModels;
}

void ASPSolver::printStats()
{
    auto statistics = this->clingo->statistics();

    stringstream ss;
    ss << "Solve Statistics:" << endl;
    ss << "TOTAL Time: " << statistics["sumary"]["times"]["total"] << "s" << endl;
    ss << "CPU Time: " << statistics["sumary"]["times"]["cpu"] << "s" << endl;
    ss << "SAT Time: " << (statistics["sumary"]["times"]["sat"] * 1000.0) << "ms" << endl;
    ss << "UNSAT Time: " << (statistics["sumary"]["times"]["unsat"] * 1000.0) << "ms" << endl;
    ss << "SOLVE Time: " << (statistics["sumary"]["times"]["solve"] * 1000.0) << "ms" << endl;

    cout << ss.str() << flush;
}

vector<shared_ptr<ASPQuery>> ASPSolver::getRegisteredQueries()
{
    return registeredQueries;
}

} /* namespace reasoner */
