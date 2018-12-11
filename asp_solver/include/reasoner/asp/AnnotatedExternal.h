#pragma once

#include <clingo.hh>

#include <memory>

namespace reasoner
{
namespace asp
{

class AnnotatedExternal
{
public:
    AnnotatedExternal(std::string aspPredicate, std::shared_ptr<Clingo::Symbol> gringoValue, bool value);
    virtual ~AnnotatedExternal();
    std::string getAspPredicate();
    void setAspPredicate(std::string aspPredicate);
    std::shared_ptr<Clingo::Symbol> getGringoValue();
    void setGringoValue(std::shared_ptr<Clingo::Symbol> gringoValue);
    bool getValue();
    void setValue(bool value);

private:
    std::string aspPredicate;
    std::shared_ptr<Clingo::Symbol> gringoValue;
    bool value;
};
} /* namespace asp */
} /* namespace reasoner */
