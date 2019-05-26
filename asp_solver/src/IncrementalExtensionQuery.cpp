#include "reasoner/asp/IncrementalExtensionQuery.h"
#include "reasoner/asp/Solver.h"
namespace reasoner
{
namespace asp
{

std::vector<IncrementalExtensionQuery*> IncrementalExtensionQuery::queries = std::vector<IncrementalExtensionQuery*>();
int IncrementalExtensionQuery::queryId = 0;

//#define QUERY_DEBUG
void IncrementalExtensionQuery::cleanUp()
{
    // Hack - first is base term query
    for (int i = 1; i < IncrementalExtensionQuery::queries.size(); ++i) {
        auto query = IncrementalExtensionQuery::queries.at(i);
        query->getSolver()->assignExternal(*(query->external), Clingo::TruthValue::False);
    }
}

bool IncrementalExtensionQuery::isPresent(int horizon)
{
    // FIXME first is base term query
    return IncrementalExtensionQuery::queries.size() > horizon;
}

void IncrementalExtensionQuery::activate(int horizon)
{

    auto query = IncrementalExtensionQuery::queries.at(horizon);
    query->getSolver()->assignExternal(*(query->external), Clingo::TruthValue::True);
}
IncrementalExtensionQuery::IncrementalExtensionQuery(Solver* solver, Term* term)
        : ExtensionQuery(solver, term)

{

    IncrementalExtensionQuery::queries.push_back(this);

    this->type = QueryType::IncrementalExtension;
    std::stringstream ss;
    if (term->getQueryId() == -1) {
#ifdef QUERY_DEBUG
        std::cout << "IncrementalQuery: Error please set the queryId and add it to any additional Fact or Rule that is going to be queried! " << std::endl;
#endif
        return;
    }

    ss << "incquery"; // << IncrementalExtensionQuery::queryId;
    this->queryProgramSection = ss.str();
    this->lastQuerySection = "";

    Clingo::SymbolVector paramsVec;
    for (auto param : this->term->getProgramSectionParameters()) {
        paramsVec.push_back(this->solver->parseValue(param.second));
        // TODO re-work, used for horizon externals

        this->lastQuerySection = this->queryProgramSection + std::to_string(stoi(param.second) - 1);
        this->queryProgramSection = this->queryProgramSection + param.second;
    }
    if (term->getProgramSectionParameters().empty()) {
        this->queryProgramSection = this->queryProgramSection + "0";
    }

    this->createProgramSection();
    this->solver->ground({{this->queryProgramSection.c_str(), paramsVec}}, nullptr);
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}

void IncrementalExtensionQuery::onModel(Clingo::Model& clingoModel)
{
    // incremental query was successful, so the next inc query can be queried
    IncrementalExtensionQuery::queryId += 1;
    //IncrementalExtensionQuery::cleanUp();
}

void IncrementalExtensionQuery::createProgramSection()
{

    std::stringstream ss;
    // hacky - how to distinguish normal loading of bg knowledge??
    if (!term->getProgramSectionParameters().empty() && !term->getRules().empty()) {
        ss << "(";
        for (int i = 0; i < term->getProgramSectionParameters().size(); ++i) {
            ss << term->getProgramSectionParameters().at(i).first;
            if (i != term->getProgramSectionParameters().size() - 1) {
                ss << ", ";
            }
        }
        ss << ")";
    }

    auto wrappedParams = ss.str();
    auto programSection = this->queryProgramSection + wrappedParams;
    this->externalName = "external" + this->queryProgramSection;

    std::stringstream queryProgram;
    queryProgram << "#program " << programSection << ".\n";
    queryProgram << "#external " << externalName << ".\n";

    for (auto& fact : term->getFacts()) {
        queryProgram << expandFactModuleProperty(fact);
        extractHeadPredicates(fact);
    }
    if (!term->getQueryRule().empty()) {
        extractHeadPredicates(term->getQueryRule());
    }
    for (auto& rule : term->getRules()) {
        extractHeadPredicates(rule);
    }

    for (auto& headPredicate : predicatesToAritiesMap) {
        for (auto& arity : headPredicate.second) {
            queryProgram << createKBCapturingRule(
                    headPredicate.first, arity,this->queryProgramSection);
        }
    }
#ifdef QUERY_DEBUG
    std::cout << "INPUT PROGRAM: " << std::endl;
    std::cout << term->getQueryRule() << std::endl;
    for (auto& rule : term->getRules()) {
        std::cout << rule << std::endl;
    }
    for (auto& fact : term->getFacts()) {
        std::cout << fact << std::endl;
    }
    std::cout << std::endl;

    std::cout << "HEAD VALUES: " << std::endl;
    for (auto& predicateArityPair : predicatesToAritiesMap) {
        std::cout << predicateArityPair.first << ": ";
        for (auto& arity : predicateArityPair.second) {
            std::cout << arity << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
#endif

    if (!term->getQueryRule().empty()) {
        queryProgram << expandRuleModuleProperty(term->getQueryRule(), (this->lastQuerySection.empty() ? this->queryProgramSection : this->lastQuerySection));
    }
    for (auto& rule : term->getRules()) {
        queryProgram << expandRuleModuleProperty(rule, (this->lastQuerySection.empty() ? this->queryProgramSection : this->lastQuerySection));
    }

#ifdef QUERY_DEBUG
    std::cout << "RESULT: " << std::endl << queryProgram.str() << std::endl;
#endif

    this->solver->add(this->queryProgramSection.c_str(), {}, queryProgram.str().c_str());
    this->external = std::make_shared<Clingo::Symbol>(this->solver->parseValue(this->externalName));
}

void IncrementalExtensionQuery::removeExternal()
{
    this->solver->assignExternal(*(this->external.get()), Clingo::TruthValue::False);
}
}
}