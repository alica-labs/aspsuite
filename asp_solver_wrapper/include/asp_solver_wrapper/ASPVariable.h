#pragma once

#include "ASPTerm.h"
#include <alica_solver_interface/SolverVariable.h>
#include <memory>
#include <string>

namespace alica
{
namespace reasoner
{

class ASPVariable : public alica::reasoner::ASPTerm, public alica::SolverVariable
{
public:
    ASPVariable(uint64_t id);
    virtual ~ASPVariable();

private:
    uint64_t id;
};

} /* namespace reasoner */
} /* namespace alica */
