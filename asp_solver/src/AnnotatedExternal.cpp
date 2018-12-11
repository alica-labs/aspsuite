#include "reasoner/asp/AnnotatedExternal.h"

namespace reasoner
{
namespace asp
{

AnnotatedExternal::AnnotatedExternal(std::string aspPredicate, std::shared_ptr<Clingo::Symbol> gringoValue, bool value)
{
    this->aspPredicate = aspPredicate;
    this->gringoValue = gringoValue;
    this->value = value;
}

AnnotatedExternal::~AnnotatedExternal() {}

std::string AnnotatedExternal::getAspPredicate()
{
    return this->aspPredicate;
}

void AnnotatedExternal::setAspPredicate(std::string aspPredicate)
{
    this->aspPredicate = aspPredicate;
}

std::shared_ptr<Clingo::Symbol> AnnotatedExternal::getGringoValue()
{
    return this->gringoValue;
}

void AnnotatedExternal::setGringoValue(std::shared_ptr<Clingo::Symbol> gringoValue)
{
    this->gringoValue = gringoValue;
}

bool AnnotatedExternal::getValue()
{
    return this->value;
}

void AnnotatedExternal::setValue(bool value)
{
    this->value = value;
}
} /* namespace asp */
} /* namespace reasoner */
