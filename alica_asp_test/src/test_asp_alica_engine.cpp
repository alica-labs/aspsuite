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

		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae));
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
			<< "Unable to initialise the Alica Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan =  ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	aspSolver->registerQuery("brokenState(wildcard)");
	aspSolver->registerQuery("topLevelPlan(wildcard)");

	if (aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: Model found!" << endl;
	}
	else
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}

	EXPECT_TRUE(aspSolver->isTrue("topLevelPlan(wildcard)")) << "Didn't find a top level plan in plan tree of '" << plan->getName() << "'";
	EXPECT_TRUE(aspSolver->isTrue("brokenState(wildcard)")) << "Didn't find a broken State in Plan '" << plan->getName() << "'";

	// stop time measuerment and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

/**
 * Tests the validation of ALICA plans
 */
//TEST_F(AspAlicaEngine, getSolutionTest)
//{
//	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "MasterPlan", ".", false))
//			<< "Unable to initialise the Alica Engine!";
//
//	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
//	alica::Plan* plan =  ae->getPlanBase()->getMasterPlan();
//	alica::ConstraintQuery query(ae);
//
//	// TODO
////	if (aspSolver->getSolution(,,))
////	{
////
////	}
//
//	//EXPECT_TRUE(aspSolver->validatePlan(plan)) << "MasterPlan '" << plan->getName() << "' is invalid!";
//}

