#pragma once

#include "reasoner/asp/ExtensionQuery.h"

namespace reasoner
{
namespace asp
{
class Solver;
class Term;
class ExtensionQuery;
class ReusableExtensionQuery : public ExtensionQuery
{

public:
    ReusableExtensionQuery(Solver* solver, Term* term);
    void removeExternal() override;
    void reactivate();
};
} /* namespace asp*/
} /* namespace reasoner */