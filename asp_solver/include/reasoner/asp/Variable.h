#pragma once

#include "reasoner/asp/Term.h"

#include <memory>
#include <string>

namespace reasoner
{
    namespace asp
    {

class Variable : public Term
{
public:
    Variable();
    virtual ~Variable();

private:
    int ownId;
    static int var_id;
};

    } /* namespace asp */
} /* namespace reasoner */
