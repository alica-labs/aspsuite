#pragma once

#include <clingo.hh>

#include <memory>
#include <string>
#include <vector>

namespace reasoner
{
class ASPCommonsTerm;
class ASPCommonsVariable;
class ASPQuery;
class AnnotatedValVec;

class IASPSolver : public std::enable_shared_from_this<IASPSolver>
{
public:
    static const void* const WILDCARD_POINTER;
    static const std::string WILDCARD_STRING;

    IASPSolver();
    virtual ~IASPSolver();

    virtual bool existsSolution(std::vector<ASPCommonsVariable*>& vars, std::vector<ASPCommonsTerm*>& calls) = 0;
    virtual bool getSolution(std::vector<ASPCommonsVariable*>& vars, std::vector<ASPCommonsTerm*>& calls, std::vector<AnnotatedValVec*>& results) = 0;
    virtual std::shared_ptr<ASPCommonsVariable> createVariable(int64_t representingVariableId) = 0;

    virtual bool loadFileFromConfig(std::string configKey) = 0;
    virtual void loadFile(std::string filename) = 0;

    virtual void ground(Clingo::PartSpan parts, Clingo::GroundCallback = nullptr) = 0;
    virtual void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue) = 0;
    virtual void releaseExternal(Clingo::Symbol ext) = 0;
    virtual bool solve() = 0;
    virtual void add(char const* name, Clingo::StringSpan const& params, char const* par) = 0;
    virtual Clingo::Symbol parseValue(std::string const& str) = 0;
    virtual int getQueryCounter() = 0;

    virtual void removeDeadQueries() = 0;
    virtual bool registerQuery(std::shared_ptr<ASPQuery> query) = 0;
    virtual bool unregisterQuery(std::shared_ptr<ASPQuery> query) = 0;
    virtual void printStats() = 0;
    virtual std::vector<std::shared_ptr<ASPQuery>> getRegisteredQueries() = 0;
};

} /* namespace reasoner */
