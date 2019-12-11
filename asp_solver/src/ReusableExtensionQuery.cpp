#include "reasoner/asp/ReusableExtensionQuery.h"
#include <reasoner/asp/Solver.h>

namespace reasoner
{
namespace asp
{

ReusableExtensionQuery::ReusableExtensionQuery(int queryID, reasoner::asp::Solver* solver, reasoner::asp::Term* term)
        : ExtensionQuery(queryID, solver, term)
{
    this->type = QueryType::ReusableExtension;
    std::stringstream ss;
    ss << "query" << queryID;
    this->queryProgramSection = ss.str();

    this->generateQueryProgram();

    Clingo::SymbolVector paramsVec;
    for (auto param : this->term->getProgramSectionParameters()) {
        paramsVec.push_back(this->solver->parseValue(param.second));
    }

    this->solver->ground({{this->queryProgramSection.c_str(), paramsVec}}, nullptr);
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}

// TODO strange naming for this query type
void ReusableExtensionQuery::removeExternal()
{
    //    std::cout << "Reusable Ext. Query " << this->term->getId() << " unregistered" << std::endl;
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::False);
}

void ReusableExtensionQuery::reactivate()
{
    //    std::cout << "Reusable Ext. Query " << this->term->getId() << " reactivated" << std::endl;
    //    // FIXME how to handle?
    //    this->setLifeTime(1);
    //    std::cout << "RExt: solver is " << (this->solver == nullptr ?  "null" : "not null") << std::endl;
    //    this->getTerm()->setLifeTime(1);
    //    std::cout << "Rext: external is " << (this->external == nullptr ? "null" : "not null") << std::endl;
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
    //    std::cout << "reactivated query" << std::endl;
}
} /* namespace asp */
} /* namespace reasoner */