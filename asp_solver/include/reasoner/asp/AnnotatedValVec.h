#pragma once

#include <clingo.hh>
#include <memory>

namespace reasoner
{
namespace asp
{
class Query;
class AnnotatedValVec
{
public:
    AnnotatedValVec(long id, std::vector<Clingo::SymbolVector> values, std::shared_ptr<Query> query);
    virtual ~AnnotatedValVec();
    long id;
    std::vector<std::vector<std::string>> variableQueryValues;
    std::vector<std::vector<std::string>> factQueryValues;
    std::shared_ptr<Query> query;

private:
    std::vector<Clingo::SymbolVector> values;
    void extractResponse();
};

} /* namespace asp */
} /* namespace reasoner */
