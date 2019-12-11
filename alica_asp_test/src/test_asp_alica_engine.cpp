#include <gtest/gtest.h>
//#include <ros/ros.h>
#include <SystemConfig.h>

// ALICA Additional Modules
#include <clock/AlicaSystemClock.h>
#include <communication/AlicaDummyCommunication.h>
#include <supplementary/AgentIDManager.h>

// ALICA Model and Engine Stuff
#include "BehaviourCreator.h"
#include "ConditionCreator.h"
#include "ConstraintCreator.h"
#include "UtilityFunctionCreator.h"
#include <engine/AlicaEngine.h>
#include <engine/PlanBase.h>
#include <engine/model/Plan.h>
#include <engine/model/PlanType.h>
#include <engine/model/State.h>
#include <engine/parser/PlanParser.h>

// ALICA ASP Solver
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/IASPSolver.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_solver/ASPSolver.h>
#include <asp_solver_wrapper/ASPGenerator.h>
#include <asp_solver_wrapper/ASPSolverWrapper.h>

#include <engine/constraintmodul/Query.h>

class AspAlicaEngine : public ::testing::Test
{

  protected:
    supplementary::SystemConfig *sc;
    alica::AlicaEngine *ae;
    alica::BehaviourCreator *bc;
    alica::ConditionCreator *cc;
    alica::UtilityFunctionCreator *uc;
    alica::ConstraintCreator *crc;
    std::chrono::_V2::system_clock::time_point start;

    virtual void SetUp()
    {
        // determine the path to the test config
        string path = supplementary::FileSystem::getSelfPath();
        int place = path.rfind("devel");
        path = path.substr(0, place);
        path = path + "src/symrock/alica_asp_test/src";

        // bring up the SystemConfig with the corresponding path
        sc = supplementary::SystemConfig::getInstance();
        sc->setRootPath(path);
        sc->setConfigPath(path + "/etc");
        sc->setHostname("nase");

        // setup the engine

        bc = new alica::BehaviourCreator();
        cc = new alica::ConditionCreator();
        uc = new alica::UtilityFunctionCreator();
        crc = new alica::ConstraintCreator();

        start = std::chrono::high_resolution_clock::now(); // start time measurement

        // TODO: is needed for AspAlicaEngine, validateWM16
        //		vector<char*> test;
        //		string bla ="hallo";
        //		test.push_back((char*)bla.c_str());
        //		int number = 1;
        //		ros::init(number,test.data(),"test");
    }

    virtual void TearDown()
    {
        // stop time measurement and report
        std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
        cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms"
             << endl;
        ae->shutdown();
        sc->shutdown();
        delete ae->getIAlicaClock();
        delete ae->getCommunicator();
        delete cc;
        delete bc;
        delete uc;
        delete crc;
    }
};

/**
 * Tests the validation of ALICA plans
 */
// TEST_F(AspAlicaEngine, validateWM16)
//{
//	//TODO how to get this test running
//  // - start roscore
//	// - uncomment the lines in setup
//	// - add msl_expressions msl_worldmodel to find package in CMakeList
//	// - replace include and src in alica_asp_test/src/etc/Expr with the ones from msl_expressions
//	// - replace nase folder in alica_asp_test/src/etc/ with the one from cnc-msl and put in missing configs
//
//	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "WM16", ".", false))
//			<< "Unable to initialise the ALICA Engine!";
//	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for
// ASPSolver
//	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();
//	// start time measurement
//
//	alica::State* brokenState = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1413992564409];
//	string queryString = aspSolver->gen.brokenState(brokenState, false);
//	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver,
// queryString,
//																									1);
//	aspSolver->registerQuery(queryObject);
//
//
//	std::chrono::_V2::system_clock::time_point start1 = std::chrono::high_resolution_clock::now();
//	auto ret = aspSolver->validatePlan(plan);
//	std::chrono::_V2::system_clock::time_point end1 = std::chrono::high_resolution_clock::now();
//	cout << "Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end1 - start1).count() << " ms"
//<< endl;
//	if (!ret)
//	{
//		cout << "ASPAlicaTest: No Model found!" << endl;
//	}
//	else
//	{
//		aspSolver->printStats();
//	}
//
//	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject)) << "The state '" << brokenState->getName() << "' should
// be broken.";
//
//	// stop time measurement and report
//	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
//	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" <<
// endl;
//}

TEST_F(AspAlicaEngine, singleUnconnectedState)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()), "Roleset",
                                "SingleUnconnectedState", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);

    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::State *brokenState = (alica::State *)(*ae->getPlanParser()->getParsedElements())[1452783421980];
    string queryString = aspSolver->gen->brokenState(brokenState, false);
    auto aspTerm = make_shared<::reasoner::ASPCommonsTerm>();
    aspTerm->addRule(queryString);
    aspTerm->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((::reasoner::ASPSolver *)(aspSolver->getSolver())), aspTerm);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The state '" << brokenState->getName()
                                                       << "' should be broken.";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, localInconsistentCardinalities)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()), "Roleset",
                                "LocalInconsistentCardinalities", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);

    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::EntryPoint *brokenEntryPoint =
        (alica::EntryPoint *)(*ae->getPlanParser()->getParsedElements())[1453033347286];
    string queryString = aspSolver->gen->brokenEntryPoint(brokenEntryPoint, false);
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);
    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;
    cout << queryObject->toString() << endl;
    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The EntryPoint '" << brokenEntryPoint->getId()
                                                       << "' should be broken";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, taskTwiceInPlan)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()), "Roleset",
                                "TaskTwicePlan", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    string queryString =
        aspSolver->gen->brokenPlanTaskPair(plan, (alica::Task *)::reasoner::IASPSolver::WILDCARD_POINTER, false);

    // gen"brokenPlanTaskPair(p1453033761283, wildcard)";
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    aspSolver->registerQuery(queryObject);
    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;
    cout << queryObject->toString() << endl;
    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "Didn't find a broken Plan-Task pair in '" << plan->getName()
                                                       << "'.";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, unconnectedStateMachine)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()), "Roleset",
                                "UnconnectedStateMachine", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::State *brokenState1 = (alica::State *)(*ae->getPlanParser()->getParsedElements())[1452783558495];
    string queryString1 = aspSolver->gen->brokenState(brokenState1, false);
    auto constraint1 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint1->addRule(queryString1);
    constraint1->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject1 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint1);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject1);

    alica::State *brokenState2 = (alica::State *)(*ae->getPlanParser()->getParsedElements())[1452783576711];
    string queryString2 = aspSolver->gen->brokenState(brokenState2, false);
    auto constraint2 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint2->addRule(queryString2);
    constraint2->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject2 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint2);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject2);

    alica::State *brokenState3 = (alica::State *)(*ae->getPlanParser()->getParsedElements())[1452783579086];
    string queryString3 = aspSolver->gen->brokenState(brokenState3, false);
    auto constraint3 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint3->addRule(queryString3);
    constraint3->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject3 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint3);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject3);

    alica::State *brokenState4 = (alica::State *)(*ae->getPlanParser()->getParsedElements())[1452783583119];
    string queryString4 = aspSolver->gen->brokenState(brokenState4, false);
    auto constraint4 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint4->addRule(queryString4);
    constraint4->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject4 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint4);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject4);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject1->factsExistForAllModels()) << "The state '" << brokenState1->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject2->factsExistForAllModels()) << "The state '" << brokenState2->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject3->factsExistForAllModels()) << "The state '" << brokenState3->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject4->factsExistForAllModels()) << "The state '" << brokenState4->getName()
                                                        << "' should be broken.";
    cout << queryObject1->toString() << endl;
    cout << queryObject2->toString() << endl;
    cout << queryObject3->toString() << endl;
    cout << queryObject4->toString() << endl;
}

TEST_F(AspAlicaEngine, hierarchicalInconsistentCardinalities)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "HierarchicalInconsistentCardinalities", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    string queryString = aspSolver->gen->brokenRunningPlan(14695984337881541968ul, false);
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The running plan 'rp" << to_string(14695984337881541968ul)
                                                       << "' should be broken.";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, cycleInPlan)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()), "Roleset",
                                "CyclePlanA", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::Plan *brokenPlan1 = (alica::Plan *)(*ae->getPlanParser()->getParsedElements())[1453033636578];
    string queryString1 = aspSolver->gen->cyclic(brokenPlan1, false);
    auto constraint1 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint1->addRule(queryString1);
    constraint1->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject1 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint1);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject1);

    alica::Plan *brokenPlan2 = (alica::Plan *)(*ae->getPlanParser()->getParsedElements())[1453033643893];
    string queryString2 = aspSolver->gen->cyclic(brokenPlan2, false);
    auto constraint2 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint2->addRule(queryString2);
    constraint2->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject2 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint2);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject2);

    alica::Plan *brokenPlan3 = (alica::Plan *)(*ae->getPlanParser()->getParsedElements())[1453033651069];
    string queryString3 = aspSolver->gen->cyclic(brokenPlan3, false);
    auto constraint3 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint3->addRule(queryString3);
    constraint3->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject3 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint3);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject3);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject1->factsExistForAllModels()) << "The plan '" << brokenPlan1->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject2->factsExistForAllModels()) << "The plan '" << brokenPlan2->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject3->factsExistForAllModels()) << "The plan '" << brokenPlan3->getName()
                                                        << "' should be broken.";
    cout << queryObject1->toString() << endl;
    cout << queryObject2->toString() << endl;
    cout << queryObject3->toString() << endl;
}

TEST_F(AspAlicaEngine, unconnectedSynchronisations)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "UnconnectedSyncRoleset", "UnconnectedSynchronisations", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::SyncTransition *brokenSynchronisation1 =
        (alica::SyncTransition *)(*ae->getPlanParser()->getParsedElements())[1455035803877];
    string queryString1 = aspSolver->gen->brokenSynchronisation(brokenSynchronisation1, false);
    auto constraint1 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint1->addRule(queryString1);
    constraint1->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject1 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint1);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject1);

    alica::SyncTransition *brokenSynchronisation2 =
        (alica::SyncTransition *)(*ae->getPlanParser()->getParsedElements())[1455035803877];
    string queryString2 = aspSolver->gen->brokenSynchronisation(brokenSynchronisation2, false);
    auto constraint2 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint2->addRule(queryString2);
    constraint2->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject2 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint2);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject2);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject1->factsExistForAllModels()) << "The synchronisation '" << brokenSynchronisation1->getName()
                                                        << "' should be broken.";
    EXPECT_TRUE(queryObject2->factsExistForAllModels()) << "The synchronisation '" << brokenSynchronisation2->getName()
                                                        << "' should be broken.";
    cout << queryObject1->toString() << endl;
    cout << queryObject2->toString() << endl;
}

TEST_F(AspAlicaEngine, reusePlanWithoutCycle)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "ReusePlanWithoutCycle", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::Plan *brokenPlan = (alica::Plan *)(*ae->getPlanParser()->getParsedElements())[1455093185652];
    string queryString1 = aspSolver->gen->brokenPlan(brokenPlan, false);
    auto constraint1 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint1->addRule(queryString1);
    constraint1->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject1 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint1);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject1);

    string queryString2 = aspSolver->gen->cyclic(brokenPlan, false);
    auto constraint2 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint2->addRule(queryString2);
    constraint2->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject2 =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint2);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject2);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_FALSE(queryObject1->factsExistForAllModels()) << "The plan '" << brokenPlan->getName()
                                                         << "' should NOT be broken.";
    EXPECT_FALSE(queryObject2->factsExistForAllModels()) << "The plan '" << brokenPlan->getName()
                                                         << "' should NOT contain a cycle.";
    cout << queryObject1->toString() << endl;
    cout << queryObject2->toString() << endl;
}

TEST_F(AspAlicaEngine, nonLocalInRelation)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "NonLocalInRelation", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    alica::PreCondition *nonLocalCondition =
        (alica::PreCondition *)(*ae->getPlanParser()->getParsedElements())[1456731822708];
    string queryString = aspSolver->gen->neglocal(nonLocalCondition, false);
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    cout << queryString << endl;
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The condition '" << nonLocalCondition->getName()
                                                       << "' should be -local(cond).";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, indirectReusePlanInPlantype)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "IndirectReusePlanInPlantype", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    string queryString = aspSolver->gen->cyclic(plan, false);
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The plan '" << plan->getName()
                                                       << "' should contain a cycle.";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, reusePlanFromPlantypeWithoutCycle)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "ReusePlanFromPlantypeWithoutCycle", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    string queryString = aspSolver->gen->cycleFree(plan, false);
    auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
    constraint->addRule(queryString);
    constraint->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> queryObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(queryObject);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The plan '" << plan->getName()
                                                       << "' should be free of cycles.";
    cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, inconsistentCardinalities)
{
    ae = new alica::AlicaEngine(new supplementary::AgentIDManager(new supplementary::AgentIDFactory()),
                                "ReusePlanWithoutCycle", "InconsistentCardinalities", ".", false);
    ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
    ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));
    // "1" stands for the ASPSolver in this test suite only!
    std::vector<char const *> args{"clingo", "-W", "no-atom-undefined", nullptr};
    auto solver = new ::reasoner::ASPSolver(args);
    auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
    solverWrapper->init(solver);
    ae->addSolver(1, solverWrapper);
    EXPECT_TRUE(ae->init(bc, cc, uc, crc)) << "Unable to initialise the ALICA Engine!";

    alica::reasoner::ASPSolverWrapper *aspSolver =
        dynamic_cast<alica::reasoner::ASPSolverWrapper *>(ae->getSolver(1)); // "1" for ASPSolver
    alica::Plan *plan = ae->getPlanBase()->getMasterPlan();

    string brokenRunningMasterPlan = aspSolver->gen->brokenRunningPlan(1922052048482545739ul, false);
    auto constraint1 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint1->addRule(brokenRunningMasterPlan);
    constraint1->setType(::reasoner::ASPQueryType::Facts);
    string brokenRunningPlan1 = aspSolver->gen->brokenRunningPlan(14695982138858284394ul, false);
    auto constraint2 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint2->addRule(brokenRunningPlan1);
    constraint2->setType(::reasoner::ASPQueryType::Facts);
    string brokenRunningPlan2 = aspSolver->gen->brokenRunningPlan(1893175574616393372ul, false);
    auto constraint3 = make_shared<::reasoner::ASPCommonsTerm>();
    constraint3->addRule(brokenRunningPlan2);
    constraint3->setType(::reasoner::ASPQueryType::Facts);
    shared_ptr<::reasoner::ASPFactsQuery> brokenRunningMasterPlanObject =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint1);
    shared_ptr<::reasoner::ASPFactsQuery> brokenRunningPlan1Object =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint2);
    shared_ptr<::reasoner::ASPFactsQuery> brokenRunningPlan2Object =
        make_shared<::reasoner::ASPFactsQuery>(((reasoner::ASPSolver *)(aspSolver->getSolver())), constraint3);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(brokenRunningMasterPlanObject);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(brokenRunningPlan1Object);
    (reasoner::ASPSolver *)(aspSolver->getSolver())->registerQuery(brokenRunningPlan2Object);

    // start time measurement for grounding
    std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
    if (!((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->validatePlan(plan))
    {
        cout << "ASPAlicaTest: No Model found!" << endl;
    }
    else
    {
        ((alica::reasoner::ASPSolverWrapper *)(aspSolver->getSolver()))->printStats();
    }
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    cout << "Measured Grounding Time: "
         << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

    EXPECT_FALSE(brokenRunningMasterPlanObject->factsExistForAllModels())
        << "The query '" << brokenRunningMasterPlan << "' (instance of master plan) should be false.";
    EXPECT_TRUE(brokenRunningPlan1Object->factsExistForAllModels()) << "The query '" << brokenRunningPlan1
                                                                    << "' should be true.";
    EXPECT_FALSE(brokenRunningPlan2Object->factsExistForAllModels()) << "The query '" << brokenRunningPlan2
                                                                     << "' should be false.";
    cout << brokenRunningMasterPlanObject->toString() << endl;
    cout << brokenRunningPlan1Object->toString() << endl;
    cout << brokenRunningPlan2Object->toString() << endl;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
