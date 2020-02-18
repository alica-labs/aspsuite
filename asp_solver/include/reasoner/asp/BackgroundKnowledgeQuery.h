#pragma once

#include "reasoner/asp/Query.h"

namespace reasoner
{
namespace asp
{
class BackgroundKnowledgeQuery : public Query
{
public:
    BackgroundKnowledgeQuery(int queryID, Solver* solver, Term* term);
    ~BackgroundKnowledgeQuery() override = default;
    void removeExternal() override; /**< NOOP in case of BackgroundKnowledgeQuery */
    std::string getBackgroundKnowledgeFilename();
    std::string getBackgroundKnowledgeProgramSection();
private:
    std::string backgroundKnowledgeProgramSection;
    std::string backgroundKnowledgeFilename;
};
} // namespace asp
} // namespace reasoner
