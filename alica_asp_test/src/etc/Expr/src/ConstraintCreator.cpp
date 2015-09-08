#include "ConstraintCreator.h"
#include <iostream>

#include  "Plans/constraints/UnconnectedState1441701808444Constraints.h"

#include  "Plans/constraints/MasterPlan1441701747919Constraints.h"

using namespace std;
using namespace alicaAutogenerated;

namespace alica
{

    ConstraintCreator::ConstraintCreator()
    {
    }

    ConstraintCreator::~ConstraintCreator()
    {
    }

    shared_ptr<BasicConstraint> ConstraintCreator::createConstraint(long constraintConfId)
    {
        switch (constraintConfId)
        {

            default:
                cerr << "ConstraintCreator: Unknown constraint requested: " << constraintConfId << endl;
                throw new exception();
                break;
        }
    }

}