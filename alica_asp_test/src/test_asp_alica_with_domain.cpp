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
		start = std::chrono::high_resolution_clock::now(); // start time measurement

		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae, args));
		alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

		aspSolver->loadFileFromConfig("assistanceBackgroundKnowledgeFile");
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
 * Tests the validation of ALICA plans with domain specific knowledge
 */
TEST_F(AspAlicaEngineWithDomain, AgentInTwoStatesOfSamePlan)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	aspSolver->loadFileFromConfig("assistanceTestFactsFile");

	aspSolver->ground( { {"assistanceTestFacts", {}}}, nullptr);
	aspSolver->ground( { {"assistanceBackground", {}}}, nullptr);
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString1 = "brokenPlanBase(donatello)";
	string queryString2 = "brokenPlanBase(leonardo)";
	string queryString3 = "brokenPlanBase(raphael)";
	string queryString4 = "brokenPlanBase(michelangelo)";
	auto constraint1 = make_shared<alica::reasoner::ASPTerm>();
	constraint1->addFact(queryString1);
	constraint1->setType(alica::reasoner::ASPQueryType::Facts);
	auto constraint2 = make_shared<alica::reasoner::ASPTerm>();
	constraint2->addFact(queryString2);
	constraint2->setType(alica::reasoner::ASPQueryType::Facts);
	auto constraint3 = make_shared<alica::reasoner::ASPTerm>();
	constraint3->addFact(queryString3);
	constraint3->setType(alica::reasoner::ASPQueryType::Facts);
	auto constraint4 = make_shared<alica::reasoner::ASPTerm>();
	constraint4->addFact(queryString4);
	constraint4->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(
			aspSolver, constraint1);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject2 = make_shared<alica::reasoner::ASPFactsQuery>(
			aspSolver, constraint2);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject3 = make_shared<alica::reasoner::ASPFactsQuery>(
			aspSolver, constraint3);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject4 = make_shared<alica::reasoner::ASPFactsQuery>(
			aspSolver, constraint4);
	aspSolver->registerQuery(queryObject1);
	aspSolver->registerQuery(queryObject2);
	aspSolver->registerQuery(queryObject3);
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
	cout << "Measured Grounding Time: "
			<< std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject1)) << "The planbase of agent donatello should be broken.";
	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject2)) << "The planbase of agent leonardo should not be broken.";
	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject3)) << "The planbase of agent raphael should not be broken.";
	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject4))
			<< "The planbase of agent michelangelo should not be broken.";
}

TEST_F(AspAlicaEngineWithDomain, ReusePlanFromPlantypeWithoutCycle_PlanBase)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "ReusePlanFromPlantypeWithoutCycle", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	aspSolver->loadFileFromConfig("assistanceTestFactsFile");

	aspSolver->ground( { {"assistanceTestFacts", {}}}, nullptr);
	aspSolver->ground( { {"assistanceBackground", {}}}, nullptr);
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString1 = "brokenPlanBase(donatello)";
	auto constraint = make_shared<alica::reasoner::ASPTerm>();
	constraint->addFact(queryString1);
	constraint->setType(alica::reasoner::ASPQueryType::Facts);
	shared_ptr<alica::reasoner::ASPFactsQuery> queryObject1 = make_shared<alica::reasoner::ASPFactsQuery>(aspSolver,
																											constraint);
	aspSolver->registerQuery(queryObject1);

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
	cout << "Measured Grounding Time: "
			<< std::chrono::duration_cast<chrono::milliseconds>(end - groundingStart).count() << " ms" << endl;

	EXPECT_FALSE(aspSolver->isTrueForAllModels(queryObject1))
			<< "The plan base of agent donatello should not be broken.";
}
