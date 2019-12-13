#pragma once

#include <clingo.hh>

#include <mutex>
#include <vector>

namespace reasoner{
    namespace asp {
        class Solver;
    }
}

namespace knowledge_manager
{
class ASPKnowledgeManager
{
public:
    ASPKnowledgeManager();
    virtual ~ASPKnowledgeManager();

    reasoner::asp::Solver* getSolver();
    virtual void setSolver(reasoner::asp::Solver* solver);
    std::vector<std::string> solve(std::string queryRule, std::string programSection);
    int addInformation(std::vector<std::string>& information, int lifetime = -1);
    void revoke(int queryID);

    // ISSUE; why are these methods necessary???
    bool solve();
    void add(char const* name, Clingo::StringSpan const& params, char const* part);
    void ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack);
    Clingo::Symbol parseValue(std::string const& str);
private:
    ::reasoner::asp::Solver* solver;
    std::vector<std::string> currentSolution;
    std::mutex mtx;
};

} /* namespace knowledge_manager */
