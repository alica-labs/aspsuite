#include "Plans/LowerPlanInPlantype1462793912377.h"
using namespace alica;
/*PROTECTED REGION ID(eph1462793912377) ENABLED START*/ //Add additional using directives here
/*PROTECTED REGION END*/
namespace alicaAutogenerated
{
    //Plan:LowerPlanInPlantype

    /* generated comment
     
     Task: DefaultTask  -> EntryPoint-ID: 1462793923281

     */
    shared_ptr<UtilityFunction> UtilityFunction1462793912377::getUtilityFunction(Plan* plan)
    {
        /*PROTECTED REGION ID(1462793912377) ENABLED START*/

        shared_ptr < UtilityFunction > defaultFunction = make_shared < DefaultUtilityFunction > (plan);
        return defaultFunction;

        /*PROTECTED REGION END*/
    }

//State: LowerState in Plan: LowerPlanInPlantype

}
