#pragma once
#include <reasoner/asp/Solver.h>
#include <mutex>
#include <vector>

namespace knowledge_manager
{
class ASPKnowledgeManager
{
public:
    ASPKnowledgeManager();
    virtual ~ASPKnowledgeManager();
    void initializeSolver(::reasoner::asp::Solver* solver);
    std::vector<std::string> solve(std::string queryRule, std::string programSection);
    int addInformation(std::vector<std::string>& information, int lifetime = -1);
    bool revoke(int queryId);

private:
    ::reasoner::asp::Solver* solver;
    std::vector<std::string> currentSolution;
    std::mutex mtx;
    std::vector<int> currentQueryIDs;
};

} /* namespace knowledge_manager */
