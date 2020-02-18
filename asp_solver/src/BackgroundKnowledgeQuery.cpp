#include "reasoner/asp/BackgroundKnowledgeQuery.h"

#include "reasoner/asp/Solver.h"
#include "reasoner/asp/Term.h"

namespace reasoner
{
namespace asp
{
BackgroundKnowledgeQuery::BackgroundKnowledgeQuery(int queryID, Solver* solver, Term* term)
        : Query(queryID, solver, term, QueryType::BackgroundKnowledge)
{
    // load background knowledge file only once (it does not ground anything)
    if (!this->term->getBackgroundKnowledgeFilename().empty()) {
        this->backgroundKnowledgeFilename = this->term->getBackgroundKnowledgeFilename();
        this->solver->loadFileFromConfig(this->backgroundKnowledgeFilename);
    }

    // ground program section with params given from term
    if (!this->term->getBackgroundKnowledgeProgramSection().empty()) {
        this->backgroundKnowledgeProgramSection = term->getBackgroundKnowledgeProgramSection();
        Clingo::SymbolVector paramsVec;
        for (auto& param : this->term->getProgramSectionParameters()) {
            paramsVec.push_back(this->solver->parseValue(param.second));
        }
        this->solver->ground({{this->backgroundKnowledgeProgramSection.c_str(), paramsVec}}, nullptr);
    }
}

std::string BackgroundKnowledgeQuery::getBackgroundKnowledgeFilename()
{
    return this->backgroundKnowledgeFilename;
}

std::string BackgroundKnowledgeQuery::getBackgroundKnowledgeProgramSection()
{
    return this->backgroundKnowledgeProgramSection;
}
/**
 * NOOP in case of BackgroundKnowledgeQuery
 */
void BackgroundKnowledgeQuery::removeExternal() {}
} // namespace asp
} // namespace reasoner