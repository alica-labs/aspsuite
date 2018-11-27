#pragma once

#include "asp_solver/GrdProgramObserver.h"

#include <SystemConfig.h>
#include <asp_commons/IASPSolver.h>
#include <clingo.hh>

#include <memory>
#include <mutex>
#include <vector>

//#define ASPSolver_DEBUG
//#define ASP_TEST_RELATED

namespace reasoner
{

class AnnotatedExternal;
class ASPFilterQuery;
class ASPQuery;
class ASPExtensionQuery;
class ASPCommonsVariable;
class AnnotatedValVec;
class ASPSolver : public IASPSolver, Clingo::SolveEventHandler
{
public:
    ASPSolver(std::vector<char const*> args);
    virtual ~ASPSolver();

    bool existsSolution(std::vector<ASPCommonsVariable*>& vars, std::vector<ASPCommonsTerm*>& calls) override;
    bool getSolution(std::vector<ASPCommonsVariable*>& vars, std::vector<ASPCommonsTerm*>& calls, std::vector<AnnotatedValVec*>& results) override;

    std::shared_ptr<ASPCommonsVariable> createVariable(int64_t representingVariableId) override;

    bool loadFileFromConfig(std::string configKey) override;
    void loadFile(std::string filename) override;

    void ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack) override;

    void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue) override;
    void releaseExternal(Clingo::Symbol ext) override;
    bool solve() override;
    void add(char const* name, Clingo::StringSpan const& params, char const* par) override;
    Clingo::Symbol parseValue(std::string const& str) override;

    int getRegisteredQueriesCount();
    /**
     * The query id has to be added to any predicate in an ASPTerm with type Variable
     * which is added to the program, naming rule heads and facts!
     * An unique id is given by the ASPSolver!
     */
    int getQueryCounter() override;

    void removeDeadQueries() override;
    bool registerQuery(std::shared_ptr<ASPQuery> query) override;
    bool unregisterQuery(std::shared_ptr<ASPQuery> query) override;
    void printStats() override;

    const double getSolvingTime();
    const double getSatTime();
    const double getUnsatTime();
    const double getModelCount();
    const double getAtomCount();
    const double getBodiesCount();
    const double getAuxAtomsCount();

    static const void* getWildcardPointer();
    static const std::string& getWildcardString();
    std::vector<std::shared_ptr<ASPQuery>> getRegisteredQueries() override;
    std::vector<Clingo::SymbolVector> getCurrentModels();

    const std::string getGroundProgram() const;

    std::shared_ptr<Clingo::Control> clingo;

private:
    bool on_model(Clingo::Model& m) override;
    void reduceQueryLifeTime();
    int prepareSolution(std::vector<ASPCommonsVariable*>& vars, std::vector<ASPCommonsTerm*>& calls);
    void handleExternals(std::shared_ptr<std::map<std::string, bool>> externals);

    std::vector<long> currentQueryIds;
    std::vector<std::string> alreadyLoaded;
    std::vector<std::shared_ptr<AnnotatedExternal>> assignedExternals;
    std::vector<std::shared_ptr<ASPQuery>> registeredQueries;
    std::vector<Clingo::SymbolVector> currentModels;

    int queryCounter;
    supplementary::SystemConfig* sc;
    GrdProgramObserver observer;

protected:
    static std::mutex queryCounterMutex;

#ifdef ASPSolver_DEBUG
    int modelCount;
#endif
};
} /* namespace reasoner */
