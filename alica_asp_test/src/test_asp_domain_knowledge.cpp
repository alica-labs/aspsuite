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

class ASPDomainKnowledge : public ::testing::Test
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
		alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

		supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
		string assistanceBackgroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("assistanceBackgroundKnowledgeFile",
		NULL);
		assistanceBackgroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																					assistanceBackgroundKnowledgeFile);
		cout << "ASPSolver: " << assistanceBackgroundKnowledgeFile << endl;
		aspSolver->load(assistanceBackgroundKnowledgeFile);
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
 * Tests the assistance domain specific knowledge
 */
TEST_F(ASPDomainKnowledge, multipleObjectCarry)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	string assistanceTestFactsFile = (*sc)["ASPSolver"]->get<string>("assistanceTestFactsFile", NULL);
	assistanceTestFactsFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(), assistanceTestFactsFile);
	cout << "ASPSolver: " << assistanceTestFactsFile << endl;
	aspSolver->load(assistanceTestFactsFile);
	aspSolver->ground( { {"assistanceTestFacts", {}}}, nullptr);
	aspSolver->ground( { {"assistanceBackground", {}}}, nullptr);
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = "inconsistent(harryPotter1)";

	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString,
																									1);
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

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The book harryPotter1 should be carried by more than one agent.";
}

TEST_F(ASPDomainKnowledge, overloaded)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	string assistanceTestFactsFile = (*sc)["ASPSolver"]->get<string>("assistanceTestFactsFile", NULL);
	assistanceTestFactsFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(), assistanceTestFactsFile);
	cout << "ASPSolver: " << assistanceTestFactsFile << endl;
	aspSolver->load(assistanceTestFactsFile);
	aspSolver->ground( { {"assistanceTestFacts", {}}}, nullptr);
	aspSolver->ground( { {"assistanceBackground", {}}}, nullptr);
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	string queryString = "overloaded(leonardo)";

	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString,
																										1);
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

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The agent can't carry by more than one thing.";
}

TEST_F(ASPDomainKnowledge, largeObject)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
	string assistanceTestFactsFile = (*sc)["ASPSolver"]->get<string>("assistanceTestFactsFile", NULL);
	assistanceTestFactsFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(), assistanceTestFactsFile);
	cout << "ASPSolver: " << assistanceTestFactsFile << endl;
	aspSolver->load(assistanceTestFactsFile);
	aspSolver->ground( { {"assistanceTestFacts", {}}}, nullptr);
	aspSolver->ground( { {"assistanceBackground", {}}}, nullptr);
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = "overloaded(michelangelo)";

	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString,
																										1);
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

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject)) << "The agent can't carry a large thing.";
}
