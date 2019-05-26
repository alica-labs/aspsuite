#include "reasoner/asp/ReusableExtensionQuery.h"
#include <reasoner/asp/Solver.h>

namespace reasoner
{
namespace asp
{

ReusableExtensionQuery::ReusableExtensionQuery(reasoner::asp::Solver* solver, reasoner::asp::Term* term)
        : ExtensionQuery(solver, term)
{
        this->type = QueryType::ReusableExtension;
        std::stringstream ss;
        if (term->getQueryId() == -1) {
            return;
        }
        ss << "query" << term->getQueryId();
        this->queryProgramSection = ss.str();

        this->createProgramSection();

        Clingo::SymbolVector paramsVec;
        for (auto param : this->term->getProgramSectionParameters()) {
            paramsVec.push_back(this->solver->parseValue(param.second));
        }


        this->solver->ground({{this->queryProgramSection.c_str(), paramsVec}}, nullptr);

        this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}

void ReusableExtensionQuery::removeExternal()
{

    this->solver->assignExternal(*(this->external), Clingo::TruthValue::False);
}

void ReusableExtensionQuery::reactivate()
{
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}
} /* namespace asp */
} /* namespace reasoner */