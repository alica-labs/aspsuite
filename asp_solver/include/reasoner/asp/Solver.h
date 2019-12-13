#pragma once

#include "reasoner/asp/GrdProgramObserver.h"
#include "reasoner/asp/Term.h"

#include <SystemConfig.h>
#include <clingo.hh>

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>

//#define ASP_TEST_RELATED

namespace reasoner
{
namespace asp
{

class AnnotatedExternal;
class FilterQuery;
class Query;
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
    bool loadFileFromConfig(const std::string& configKey);
    void loadFile(const std::string& filename);
    void ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack);
    void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue);
    void releaseExternal(Clingo::Symbol ext);
    bool solve();
    void add(char const* name, Clingo::StringSpan const& params, char const* par);
    Clingo::Symbol parseValue(std::string const& str);

    /**
     * The query id has to be added to any predicate in an ASPTerm with type Variable
     * which is added to the program, naming rule heads and facts!
     * An unique id is given by the ASPSolver!
     */
    int generateQueryID();
    void removeDeadQueries();
    /**
     * Registers the given query.
     * @param query
     * @return
     */
    void registerQuery(std::shared_ptr<Query> query);
    void unregisterQuery(int queryID);
    void setNumberOfModels(int numberOfModels);
    void printStats();
    double getSolvingTime();
    double getSatTime();
    double getUnsatTime();
    double getModelCount();
    double getAtomCount();
    double getBodiesCount();
    double getAuxAtomsCount();
    const std::unordered_map<int, std::shared_ptr<Query>> getRegisteredQueries() const;
    std::vector<Clingo::SymbolVector> getCurrentModels();
    Clingo::SymbolicAtoms getSymbolicAtoms();
    const std::string getGroundProgram() const;

private:
    bool onModel(Clingo::Model& m);
    void reduceQueryLifeTime();
    int prepareSolution(std::vector<Variable*>& vars, std::vector<Term*>& calls);
    void handleExternals(std::shared_ptr<std::map<std::string, bool>> externals);

    essentials::SystemConfig* sc;
    GrdProgramObserver observer;
    Clingo::Control* clingo;

    int queryCounter; /**< Used for generating unique query IDs*/
    std::vector<std::string> alreadyLoaded; /**< Knowledge files that are already loaded*/
    std::vector<std::shared_ptr<AnnotatedExternal>> assignedExternals;
    std::unordered_map<int, std::shared_ptr<Query>> registeredQueries;
    std::vector<Clingo::SymbolVector> currentModels;

protected:
    static std::mutex queryCounterMutex; /**< Making the creation of query IDs thread-safe */
    static std::mutex clingoMtx;

#ifdef SOLVER_DEBUG
    int modelCount;
#endif
};
} /* namespace asp */
} /* namespace reasoner */
