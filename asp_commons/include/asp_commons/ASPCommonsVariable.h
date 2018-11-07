#pragma once

#include <memory>
#include <string>

#include "asp_commons/ASPCommonsTerm.h"

namespace reasoner
{

class ASPCommonsVariable : public reasoner::ASPCommonsTerm
{
public:
    ASPCommonsVariable();
    virtual ~ASPCommonsVariable();

private:
    int ownId;
    static int var_id;
};

} /* namespace reasoner */
