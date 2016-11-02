#include <gtest/gtest.h>
//#include <ros/ros.h>
#include <SystemConfig.h>

// ALICA Additional Modules
#include <communication/AlicaDummyCommunication.h>
#include <clock/AlicaSystemClock.h>

// ALICA Model and Engine Stuff
#include "BehaviourCreator.h"
#include "ConditionCreator.h"
#include "ConstraintCreator.h"
#include "UtilityFunctionCreator.h"
#include <engine/AlicaEngine.h>
#include <engine/PlanBase.h>
#include <engine/model/Plan.h>
#include <engine/model/State.h>
#include <engine/model/PlanType.h>
#include <engine/constraintmodul/ConstraintQuery.h>
#include <engine/IPlanParser.h>

// ALICA ASP Solver
#include <alica_asp_solver/ASPSolver.h>
#include <alica_asp_solver/ASPTerm.h>
#include <alica_asp_solver/ASPFactsQuery.h>
#include <alica_asp_solver/ASPFactsQuery.h>

class AspAlicaEngine : public ::testing::Test
{

protected:
	supplementary::SystemConfig* sc;
	alica::AlicaEngine* ae;
	alica::BehaviourCreator* bc;
	alica::ConditionCreator* cc;
	alica::UtilityFunctionCreator* uc;
	alica::ConstraintCreator* crc;
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
		ae = new alica::AlicaEngine();
		bc = new alica::BehaviourCreator();
		cc = new alica::ConditionCreator();
		uc = new alica::UtilityFunctionCreator();
		crc = new alica::ConstraintCreator();
		ae->setIAlicaClock(new alica_dummy_proxy::AlicaSystemClock());
		ae->setCommunicator(new alica_dummy_proxy::AlicaDummyCommunication(ae));

		std::vector<char const *> args {"clingo", "-W", "no-atom-undefined", nullptr};
		start = std::chrono::high_resolution_clock::now();// start time measurement
		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae, args));
		//TODO: is needed for AspAlicaEngine, validateWM16
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
		cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
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
//TEST_F(AspAlicaEngine, validateWM16)
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
//	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
//	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();
//	// start time measurement
//
//	alica::State* brokenState = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1413992564409];
//	string queryString = aspSolver->gen.brokenState(brokenState, false);
//	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, queryString,
//																									1);
//	aspSolver->registerQuery(queryObject);
//
//
//	std::chrono::_V2::system_clock::time_point start1 = std::chrono::high_resolution_clock::now();
//	auto ret = aspSolver->validatePlan(plan);
//	std::chrono::_V2::system_clock::time_point end1 = std::chrono::high_resolution_clock::now();
//	cout << "Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end1 - start1).count() << " ms" << endl;
//	if (!ret)
//	{
//		cout << "ASPAlicaTest: No Model found!" << endl;
//	}
//	else
//	{
//		aspSolver->printStats();
//	}
//
//	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject)) << "The state '" << brokenState->getName() << "' should be broken.";
//
//	// stop time measurement and report
//	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
//	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
//}


TEST_F(AspAlicaEngine, singleUnconnectedState)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "SingleUnconnectedState", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::State* brokenState = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783421980];
	string queryString = aspSolver->gen.brokenState(brokenState, false);
    auto constraint = make_shared<alica::reasoner::ASPTerm>();
    constraint->setRule(queryString);
    constraint->setType(alica::reasoner::ASPQueryType::Facts);
    constraint->setProgrammSection("planBase");
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The state '" << brokenState->getName() << "' should be broken.";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, localInconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "LocalInconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::EntryPoint* brokenEntryPoint = (alica::EntryPoint*)(*ae->getPlanParser()->getParsedElements())[1453033347286];
	string queryString = aspSolver->gen.brokenEntryPoint(brokenEntryPoint, false);
    auto constraint = make_shared<alica::reasoner::ASPTerm>();
    constraint->setRule(queryString);
    constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);
	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;
	cout << queryObject->toString() << endl;
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The EntryPoint '" << brokenEntryPoint->getId()
			<< "' should be broken";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, taskTwiceInPlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "TaskTwicePlan", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = aspSolver->gen.brokenPlanTaskPair(plan,
															(alica::Task*)alica::reasoner::ASPSolver::WILDCARD_POINTER,
															false);

	//gen"brokenPlanTaskPair(p1453033761283, wildcard)";
    auto constraint = make_shared<alica::reasoner::ASPTerm>();
    constraint->setRule(queryString);
    constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);
	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;
	cout << queryObject->toString() << endl;
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "Didn't find a broken Plan-Task pair in '" << plan->getName()
			<< "'.";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, unconnectedStateMachine)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "UnconnectedStateMachine", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::State* brokenState1 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783558495];
	string queryString1 = aspSolver->gen.brokenState(brokenState1, false);
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->setRule(queryString1);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint1);
	aspSolver->registerQuery(queryObject1);

	alica::State* brokenState2 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783576711];
	string queryString2 = aspSolver->gen.brokenState(brokenState2, false);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->setRule(queryString2);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject2 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint2);
	aspSolver->registerQuery(queryObject2);

	alica::State* brokenState3 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783579086];
	string queryString3 = aspSolver->gen.brokenState(brokenState3, false);
	auto constraint3 = make_shared<alica::reasoner::ASPTerm>();
	constraint3->setRule(queryString3);
	constraint3->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject3 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint3);
	aspSolver->registerQuery(queryObject3);

	alica::State* brokenState4 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783583119];
	string queryString4 = aspSolver->gen.brokenState(brokenState4, false);
	auto constraint4 = make_shared<alica::reasoner::ASPTerm>();
	constraint4->setRule(queryString4);
	constraint4->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject4 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint4);
	aspSolver->registerQuery(queryObject4);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject1)) << "The state '" << brokenState1->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject2)) << "The state '" << brokenState2->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject3)) << "The state '" << brokenState3->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject4)) << "The state '" << brokenState4->getName() << "' should be broken.";
	cout << queryObject1->toString() << endl;
	cout << queryObject2->toString() << endl;
	cout << queryObject3->toString() << endl;
	cout << queryObject4->toString() << endl;
}

TEST_F(AspAlicaEngine, hierarchicalInconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "HierarchicalInconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = aspSolver->gen.brokenRunningPlan(14695984337881541968ul, false);
	auto constraint = make_shared<alica::reasoner::ASPTerm>();
	constraint->setRule(queryString);
	constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The running plan 'rp" << to_string(14695984337881541968ul)
			<< "' should be broken.";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, cycleInPlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "CyclePlanA", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::Plan* brokenPlan1 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033636578];
	string queryString1 = aspSolver->gen.cyclic(brokenPlan1, false);
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->setRule(queryString1);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint1);
	aspSolver->registerQuery(queryObject1);

	alica::Plan* brokenPlan2 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033643893];
	string queryString2 = aspSolver->gen.cyclic(brokenPlan2, false);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->setRule(queryString2);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject2 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint2);
	aspSolver->registerQuery(queryObject2);

	alica::Plan* brokenPlan3 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033651069];
	string queryString3 = aspSolver->gen.cyclic(brokenPlan3, false);
	auto constraint3 = make_shared<alica::reasoner::ASPTerm>();
	constraint3->setRule(queryString3);
	constraint3->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject3 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint3);
	aspSolver->registerQuery(queryObject3);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject1)) << "The plan '" << brokenPlan1->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject2)) << "The plan '" << brokenPlan2->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject3)) << "The plan '" << brokenPlan3->getName() << "' should be broken.";
	cout << queryObject1->toString() << endl;
	cout << queryObject2->toString() << endl;
	cout << queryObject3->toString() << endl;
}

TEST_F(AspAlicaEngine, unconnectedSynchronisations)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "UnconnectedSyncRoleset", "UnconnectedSynchronisations", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::SyncTransition* brokenSynchronisation1 =
			(alica::SyncTransition*)(*ae->getPlanParser()->getParsedElements())[1455035803877];
	string queryString1 = aspSolver->gen.brokenSynchronisation(brokenSynchronisation1, false);
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->setRule(queryString1);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint1);
	aspSolver->registerQuery(queryObject1);

	alica::SyncTransition* brokenSynchronisation2 =
			(alica::SyncTransition*)(*ae->getPlanParser()->getParsedElements())[1455035803877];
	string queryString2 = aspSolver->gen.brokenSynchronisation(brokenSynchronisation2, false);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->setRule(queryString2);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject2 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint2);
	aspSolver->registerQuery(queryObject2);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject1)) << "The synchronisation '" << brokenSynchronisation1->getName()
			<< "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject2)) << "The synchronisation '" << brokenSynchronisation2->getName()
			<< "' should be broken.";
	cout << queryObject1->toString() << endl;
	cout << queryObject2->toString() << endl;
}

TEST_F(AspAlicaEngine, reusePlanWithoutCycle)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "ReusePlanWithoutCycle", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::Plan* brokenPlan = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1455093185652];
	string queryString1 = aspSolver->gen.brokenPlan(brokenPlan, false);
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->setRule(queryString1);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint1);
	aspSolver->registerQuery(queryObject1);

	string queryString2 = aspSolver->gen.cyclic(brokenPlan, false);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->setRule(queryString2);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject2 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint2);
	aspSolver->registerQuery(queryObject2);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject1)) << "The plan '" << brokenPlan->getName() << "' should NOT be broken.";
	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject2)) << "The plan '" << brokenPlan->getName()
			<< "' should NOT contain a cycle.";
	cout << queryObject1->toString() << endl;
	cout << queryObject2->toString() << endl;
}

TEST_F(AspAlicaEngine, nonLocalInRelation)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "NonLocalInRelation", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	alica::PreCondition* nonLocalCondition =
			(alica::PreCondition*)(*ae->getPlanParser()->getParsedElements())[1456731822708];
	string queryString = aspSolver->gen.neglocal(nonLocalCondition, false);
	auto constraint = make_shared<alica::reasoner::ASPTerm>();
	constraint->setRule(queryString);
	constraint->setType(alica::reasoner::ASPQueryType::Facts);
	cout << queryString << endl;
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The condition '" << nonLocalCondition->getName()
			<< "' should be -local(cond).";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, indirectReusePlanInPlantype)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "IndirectReusePlanInPlantype", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = aspSolver->gen.cyclic(plan, false);
	auto constraint = make_shared<alica::reasoner::ASPTerm>();
	constraint->setRule(queryString);
	constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The plan '" << plan->getName() << "' should contain a cycle.";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, reusePlanFromPlantypeWithoutCycle)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "ReusePlanFromPlantypeWithoutCycle", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = aspSolver->gen.cycleFree(plan, false);
	auto constraint = make_shared<alica::reasoner::ASPTerm>();
	constraint->setRule(queryString);
	constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint);
	aspSolver->registerQuery(queryObject);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The plan '" << plan->getName() << "' should be free of cycles.";
	cout << queryObject->toString() << endl;
}

TEST_F(AspAlicaEngine, inconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "InconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string brokenRunningMasterPlan = aspSolver->gen.brokenRunningPlan(1922052048482545739ul, false);
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->setRule(brokenRunningMasterPlan);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	string brokenRunningPlan1 = aspSolver->gen.brokenRunningPlan(14695982138858284394ul, false);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->setRule(brokenRunningPlan1);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	string brokenRunningPlan2 = aspSolver->gen.brokenRunningPlan(1893175574616393372ul, false);
	auto constraint3 = make_shared<alica::reasoner::ASPTerm>();
	constraint3->setRule(brokenRunningPlan2);
	constraint3->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> brokenRunningMasterPlanObject = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint1);
	shared_ptr<alica::reasoner::ASPFactsQuery> brokenRunningPlan1Object = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint2);
	shared_ptr<alica::reasoner::ASPFactsQuery> brokenRunningPlan2Object = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver, constraint3);
	aspSolver->registerQuery(brokenRunningMasterPlanObject);
	aspSolver->registerQuery(brokenRunningPlan1Object);
	aspSolver->registerQuery(brokenRunningPlan2Object);

	// start time measurement for grounding
	std::chrono::_V2::system_clock::time_point groundingStart = std::chrono::high_resolution_clock::now();
	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_FALSE(aspSolver->isTrueForAllModels(brokenRunningMasterPlanObject)) << "The query '" << brokenRunningMasterPlan
			<< "' (instance of master plan) should be false.";
	EXPECT_TRUE(aspSolver->isTrueForAllModels(brokenRunningPlan1Object)) << "The query '" << brokenRunningPlan1
			<< "' should be true.";
	EXPECT_FALSE(aspSolver->isTrueForAllModels(brokenRunningPlan2Object)) << "The query '" << brokenRunningPlan2
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
