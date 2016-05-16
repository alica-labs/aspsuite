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

class AspAlicaEngineWithDomain : public ::testing::Test
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
 * Tests the validation of ALICA plans with domain specific knowledge
 */
TEST_F(AspAlicaEngineWithDomain, multipleObjectCarry)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
	string assistanceBackgroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("assistanceBackgroundKnowledgeFile", NULL);
	assistanceBackgroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																				assistanceBackgroundKnowledgeFile);
	cout << "ASPSolver: " << assistanceBackgroundKnowledgeFile << endl;
	aspSolver->load(assistanceBackgroundKnowledgeFile);

	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = "inconsistent(harryPotter1)";

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The book harryPotter1 should be carried by more than one agent.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngineWithDomain, overloaded)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
	string assistanceBackgroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("assistanceBackgroundKnowledgeFile", NULL);
	assistanceBackgroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																				assistanceBackgroundKnowledgeFile);
	cout << "ASPSolver: " << assistanceBackgroundKnowledgeFile << endl;
	aspSolver->load(assistanceBackgroundKnowledgeFile);

	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = "overloaded(leonardo)";

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The agent can't carry by more than one thing.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngineWithDomain, largeObject)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
	string assistanceBackgroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("assistanceBackgroundKnowledgeFile", NULL);
	assistanceBackgroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																				assistanceBackgroundKnowledgeFile);
	cout << "ASPSolver: " << assistanceBackgroundKnowledgeFile << endl;
	aspSolver->load(assistanceBackgroundKnowledgeFile);

	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = "overloaded(michelangelo)";

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The agent can't carry a large thing.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}

TEST_F(AspAlicaEngineWithDomain, AgentInTwoStatesOfSamePlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
	string assistanceBackgroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("assistanceBackgroundKnowledgeFile", NULL);
	assistanceBackgroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																				assistanceBackgroundKnowledgeFile);
	cout << "ASPSolver: " << assistanceBackgroundKnowledgeFile << endl;
	aspSolver->load(assistanceBackgroundKnowledgeFile);

	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = "brokenPlanBase(donatello)";

	aspSolver->registerQuery(queryString);

	if (!aspSolver->validatePlan(plan))
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrue(queryString)) << "The planbase of agent donatello should be broken.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}
