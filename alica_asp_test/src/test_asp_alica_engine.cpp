#include <gtest/gtest.h>

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

class AspAlicaEngine : public ::testing::Test
{

protected:
	supplementary::SystemConfig* sc;
	alica::AlicaEngine* ae;
	alica::BehaviourCreator* bc;
	alica::ConditionCreator* cc;
	alica::UtilityFunctionCreator* uc;
	alica::ConstraintCreator* crc;

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

		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae, args));
	}

	virtual void TearDown()
	{
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
TEST_F(AspAlicaEngine, singleUnconnectedState)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "SingleUnconnectedState", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::State* brokenState = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783421980];
	string queryString = aspSolver->gen.brokenState(brokenState, false);
	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The state '" << brokenState->getName() << "' should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, localInconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "LocalInconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::EntryPoint* brokenEntryPoint = (alica::EntryPoint*)(*ae->getPlanParser()->getParsedElements())[1453033347286];
	string queryString = aspSolver->gen.brokenEntryPoint(brokenEntryPoint, false);
	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The EntryPoint '" << brokenEntryPoint->getId()
			<< "' should be broken";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, taskTwiceInPlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "TaskTwicePlan", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = aspSolver->gen.brokenPlanTaskPair(plan,
															(alica::Task*)alica::reasoner::ASPSolver::WILDCARD_POINTER,
															false);

	//gen"brokenPlanTaskPair(p1453033761283, wildcard)";
	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "Didn't find a broken Plan-Task pair in '" << plan->getName()
			<< "'.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, unconnectedStateMachine)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "UnconnectedStateMachine", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::State* brokenState1 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783558495];
	string queryString1 = aspSolver->gen.brokenState(brokenState1, false);
	aspSolver->registerQuery(queryString1);

	alica::State* brokenState2 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783576711];
	string queryString2 = aspSolver->gen.brokenState(brokenState2, false);
	aspSolver->registerQuery(queryString2);

	alica::State* brokenState3 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783579086];
	string queryString3 = aspSolver->gen.brokenState(brokenState3, false);
	aspSolver->registerQuery(queryString3);

	alica::State* brokenState4 = (alica::State*)(*ae->getPlanParser()->getParsedElements())[1452783583119];
	string queryString4 = aspSolver->gen.brokenState(brokenState4, false);
	aspSolver->registerQuery(queryString4);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString1)) << "The state '" << brokenState1->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString2)) << "The state '" << brokenState2->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString3)) << "The state '" << brokenState3->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString4)) << "The state '" << brokenState4->getName() << "' should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, hierarchicalInconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "HierarchicalInconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = aspSolver->gen.brokenRunningPlan(14695984337881541968ul, false);
	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The running plan 'rp" << to_string(14695984337881541968ul)
			<< "' should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, cycleInPlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "CyclePlanA", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::Plan* brokenPlan1 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033636578];
	string queryString1 = aspSolver->gen.cyclic(brokenPlan1, false);
	aspSolver->registerQuery(queryString1);

	alica::Plan* brokenPlan2 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033643893];
	string queryString2 = aspSolver->gen.cyclic(brokenPlan2, false);
	aspSolver->registerQuery(queryString2);

	alica::Plan* brokenPlan3 = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1453033651069];
	string queryString3 = aspSolver->gen.cyclic(brokenPlan3, false);
	aspSolver->registerQuery(queryString3);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString1)) << "The plan '" << brokenPlan1->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString2)) << "The plan '" << brokenPlan2->getName() << "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString3)) << "The plan '" << brokenPlan3->getName() << "' should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, unconnectedSynchronisations)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "UnconnectedSyncRoleset", "UnconnectedSynchronisations", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::SyncTransition* brokenSynchronisation1 =
			(alica::SyncTransition*)(*ae->getPlanParser()->getParsedElements())[1455035803877];
	string queryString1 = aspSolver->gen.brokenSynchronisation(brokenSynchronisation1, false);
	aspSolver->registerQuery(queryString1);

	alica::SyncTransition* brokenSynchronisation2 =
			(alica::SyncTransition*)(*ae->getPlanParser()->getParsedElements())[1455035803877];
	string queryString2 = aspSolver->gen.brokenSynchronisation(brokenSynchronisation2, false);
	aspSolver->registerQuery(queryString2);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString1)) << "The synchronisation '" << brokenSynchronisation1->getName()
			<< "' should be broken.";
	EXPECT_TRUE(aspSolver->isTrue(queryString2)) << "The synchronisation '" << brokenSynchronisation2->getName()
			<< "' should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, reusePlanWithoutCycle)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "ReusePlanWithoutCycle", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::Plan* brokenPlan = (alica::Plan*)(*ae->getPlanParser()->getParsedElements())[1455093185652];
	string queryString1 = aspSolver->gen.brokenPlan(brokenPlan, false);
	aspSolver->registerQuery(queryString1);

	string queryString2 = aspSolver->gen.cyclic(brokenPlan, false);
	aspSolver->registerQuery(queryString2);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_FALSE(aspSolver->isTrue(queryString1)) << "The plan '" << brokenPlan->getName() << "' should NOT be broken.";
	EXPECT_FALSE(aspSolver->isTrue(queryString2)) << "The plan '" << brokenPlan->getName()
			<< "' should NOT contain a cycle.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, nonLocalInRelation)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "NonLocalInRelation", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	alica::PreCondition* nonLocalCondition =
			(alica::PreCondition*)(*ae->getPlanParser()->getParsedElements())[1456731822708];
	string queryString = aspSolver->gen.neglocal(nonLocalCondition, false);
	cout << queryString << endl;
	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The condition '" << nonLocalCondition->getName()
			<< "' should be -local(cond).";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, indirectReusePlanInPlantype)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "IndirectReusePlanInPlantype", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = aspSolver->gen.cyclic(plan, false);

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The plan '" << plan->getName() << "' should contain a cycle.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, reusePlanFromPlantypeWithoutCycle)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "ReusePlanFromPlantypeWithoutCycle", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = aspSolver->gen.cycleFree(plan, false);

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The plan '" << plan->getName() << "' should be free of cycles.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngine, inconsistentCardinalities)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "InconsistentCardinalities", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string brokenRunningMasterPlan = aspSolver->gen.brokenRunningPlan(1922052048482545739ul, false);
	string brokenRunningPlan1 = aspSolver->gen.brokenRunningPlan(14695982138858284394ul, false);
	string brokenRunningPlan2 = aspSolver->gen.brokenRunningPlan(1893175574616393372ul, false);

	aspSolver->registerQuery(brokenRunningMasterPlan);
	aspSolver->registerQuery(brokenRunningPlan1);
	aspSolver->registerQuery(brokenRunningPlan2);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_FALSE(aspSolver->isTrue(brokenRunningMasterPlan)) << "The query '" << brokenRunningMasterPlan
			<< "' (instance of master plan) should be false.";
	EXPECT_TRUE(aspSolver->isTrue(brokenRunningPlan1)) << "The query '" << brokenRunningPlan1
			<< "' should be true.";
	EXPECT_FALSE(aspSolver->isTrue(brokenRunningPlan2)) << "The query '" << brokenRunningPlan2
			<< "' should be false.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
