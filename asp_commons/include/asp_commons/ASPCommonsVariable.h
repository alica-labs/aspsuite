#pragma once

#include "asp_commons/ASPCommonsTerm.h"

#include <memory>
#include <string>

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
