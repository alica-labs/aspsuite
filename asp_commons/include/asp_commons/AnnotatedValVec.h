#pragma once

#include <clingo.hh>
#include <memory>

namespace reasoner
{
class ASPQuery;
class AnnotatedValVec
{
public:
    AnnotatedValVec(long id, std::vector<Clingo::SymbolVector> values, std::shared_ptr<ASPQuery> query);
    virtual ~AnnotatedValVec();
    long id;
    std::vector<std::vector<std::string>> variableQueryValues;
    std::vector<std::vector<std::string>> factQueryValues;
    std::shared_ptr<ASPQuery> query;

private:
    std::vector<Clingo::SymbolVector> values;
    void extractResponse();
};

} /* namespace reasoner */
