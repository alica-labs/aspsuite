#include "ConstraintCreator.h"
#include <iostream>

#include  "Plans/constraints/LocalInconsistentCardinalities1453033347284Constraints.h"

#include  "Plans/constraints/CyclePlanB1453033643893Constraints.h"

#include  "Plans/constraints/UnconnectedStateMachine1452783558494Constraints.h"

#include  "Plans/constraints/SingleUnconnectedState1452783421979Constraints.h"

#include  "Plans/constraints/ReferencedPlan1456731591075Constraints.h"

#include  "Plans/constraints/UnconnectedSynchronisations1455035766972Constraints.h"

#include  "Plans/constraints/Min3AgentPlan1453033506291Constraints.h"

#include  "Plans/constraints/UpperPlanInPlantype1462793904808Constraints.h"

#include  "Plans/constraints/LowerPlanInPlantype1462793912377Constraints.h"

#include  "Plans/constraints/CyclePlanC1453033651069Constraints.h"

#include  "Plans/constraints/CyclePlanA1453033636578Constraints.h"

#include  "Plans/constraints/TaskTwicePlan1453033761283Constraints.h"

#include  "Plans/constraints/HierarchicalInconsistentCardinalities1453033407259Constraints.h"

#include  "Plans/constraints/NonLocalInRelation1456730912737Constraints.h"

#include  "Plans/constraints/IndirectReusePlanInPlantype1462793790517Constraints.h"

#include  "Plans/constraints/ReusePlanWithoutCycle1455093185652Constraints.h"

#include  "Plans/constraints/InRelationPlan1456731505906Constraints.h"

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
