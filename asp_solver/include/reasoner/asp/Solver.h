#pragma once

#include "reasoner/asp/GrdProgramObserver.h"
#include "reasoner/asp/Term.h"

#include <SystemConfig.h>
#include <clingo.hh>

#include <memory>
#include <mutex>
#include <vector>

//#define ASPSolver_DEBUG
//#define ASP_TEST_RELATED

namespace reasoner
{
namespace asp
{

class AnnotatedExternal;
class Query;
class FilterQuery;
class ExtensionQuery;
class Variable;
class AnnotatedValVec;
class Term;

class Solver : Clingo::SolveEventHandler
{
public:
    static const void* const WILDCARD_POINTER;
    static const std::string WILDCARD_STRING;

    Solver(std::vector<char const*> args);

    virtual ~Solver();

    bool existsSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls);

    bool getSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls, std::vector<AnnotatedValVec*>& results);

    std::shared_ptr<Variable> createVariable(int64_t representingVariableId);

    bool loadFileFromConfig(std::string configKey);

    void loadFile(std::string filename);

    void ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack);

    void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue);

    void releaseExternal(Clingo::Symbol ext);

    bool solve();

    void add(char const* name, Clingo::StringSpan const& params, char const* par);

    Clingo::Symbol parseValue(std::string const& str);

    int getRegisteredQueriesCount();

    /**
     * The query id has to be added to any predicate in an ASPTerm with type Variable
     * which is added to the program, naming rule heads and facts!
     * An unique id is given by the ASPSolver!
     */
    int getQueryCounter();

    void removeDeadQueries();

    bool registerQuery(std::shared_ptr<Query> query);

    bool unregisterQuery(std::shared_ptr<Query> query);

    void printStats();

    const double getSolvingTime();

    const double getSatTime();

    const double getUnsatTime();

    const double getModelCount();

    const double getAtomCount();

    const double getBodiesCount();

    const double getAuxAtomsCount();

    static const void* getWildcardPointer();

    static const std::string& getWildcardString();

    std::vector<std::shared_ptr<Query>> getRegisteredQueries();

    std::vector<Clingo::SymbolVector> getCurrentModels();

    const std::string getGroundProgram() const;

    std::shared_ptr<Clingo::Control> clingo;
//    Clingo::Control* clingo;
    static std::mutex clingoMtx;

private:
    bool on_model(Clingo::Model& m);

    void reduceQueryLifeTime();

    int prepareSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls);

    void handleExternals(std::shared_ptr<std::map<std::string, bool>> externals);

    std::vector<long> currentQueryIds;
    std::vector<std::string> alreadyLoaded;
    std::vector<std::shared_ptr<AnnotatedExternal>> assignedExternals;
    std::vector<std::shared_ptr<Query>> registeredQueries;
    std::vector<Clingo::SymbolVector> currentModels;

    int queryCounter;
    essentials::SystemConfig* sc;
    GrdProgramObserver observer;

protected:
    static std::mutex queryCounterMutex;

#ifdef ASPSolver_DEBUG
    int modelCount;
#endif
};
} /* namespace asp */
} /* namespace reasoner */
