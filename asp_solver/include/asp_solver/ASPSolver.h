#pragma once
#include <SystemConfig.h>
#include <clingo.hh>
#include <mutex>

#include <memory>
#include <vector>

#include <asp_commons/IASPSolver.h>

#include "asp_solver/GrdProgramObserver.h"

//#define ASPSolver_DEBUG
//#define ASP_TEST_RELATED
//#define SOLVER_OPTIONS

using namespace std;

namespace reasoner
{

class AnnotatedExternal;
class ASPFactsQuery;
class ASPQuery;
class ASPVariableQuery;
class ASPCommonsVariable;
class AnnotatedValVec;
class ASPSolver : public IASPSolver, Clingo::SolveEventHandler
{
public:
    ASPSolver(std::vector<char const*> args);
    virtual ~ASPSolver();

    bool existsSolution(vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls) override;
    bool getSolution(vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls, vector<AnnotatedValVec*>& results) override;

    shared_ptr<ASPCommonsVariable> createVariable(int64_t representingVariableId) override;

    bool loadFileFromConfig(string configKey);
    void loadFile(string filename);

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
    bool registerQuery(shared_ptr<ASPQuery> query) override;
    bool unregisterQuery(shared_ptr<ASPQuery> query) override;
    void printStats() override;

    const double getSolvingTime();
    const double getSatTime();
    const double getUnsatTime();
    const double getModelCount();
    const double getAtomCount();
    const double getBodiesCount();
    const double getAuxAtomsCount();

    static const void* getWildcardPointer();
    static const string& getWildcardString();
    vector<shared_ptr<ASPQuery>> getRegisteredQueries() override;
    vector<Clingo::SymbolVector> getCurrentModels();

    shared_ptr<Clingo::Control> clingo;

    const std::string getGroundProgram() const;

private:
    bool on_model(Clingo::Model& m) override;
    vector<long> currentQueryIds;

    vector<string> alreadyLoaded;
    vector<shared_ptr<AnnotatedExternal>> assignedExternals;
    vector<shared_ptr<ASPQuery>> registeredQueries;
    vector<Clingo::SymbolVector> currentModels;

    void reduceQueryLifeTime();
    int prepareSolution(std::vector<ASPCommonsVariable*>& vars, vector<ASPCommonsTerm*>& calls);
    int queryCounter;
    supplementary::SystemConfig* sc;
    GrdProgramObserver observer;

protected:
    static mutex queryCounterMutex;

#ifdef ASPSolver_DEBUG
    int modelCount;
#endif
};
template <class T>
void traverseOptions(T& conf, unsigned key, std::string accu)
{
    int subKeys, arrLen;
    const char* help;
    conf->getKeyInfo(key, &subKeys, &arrLen, &help);
    if (arrLen > 0) {
        for (int i = 0; i != arrLen; ++i) {
            traverseOptions(conf, conf->getArrKey(key, i), accu + std::to_string(i) + ".");
        }
    } else if (subKeys > 0) {
        for (int i = 0; i != subKeys; ++i) {
            const char* sk = conf->getSubKeyName(key, i);
            traverseOptions(conf, conf->getSubKey(key, sk), accu + sk);
        }
    } else {
        std::cout << accu << " -- " << (help ? help : "") << "\n";
    }
}

} /* namespace reasoner */
