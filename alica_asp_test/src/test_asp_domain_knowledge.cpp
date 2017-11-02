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
#include <engine/IPlanParser.h>

// ALICA ASP Solver
#include <asp_commons/IASPSolver.h>
#include <asp_solver_wrapper/ASPSolverWrapper.h>
#include <asp_solver_wrapper/ASPGenerator.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_solver/ASPSolver.h>
#include <asp_commons/ASPQueryType.h>

#include <engine/constraintmodul/Query.h>

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


		start = std::chrono::high_resolution_clock::now();// start time measurement
		// "1" stands for the ASPSolver in this test suite only!
		std::vector<char const *> args {"clingo", "-W", "no-atom-undefined", nullptr};
		auto solver = new ::reasoner::ASPSolver(args);
		auto solverWrapper = new alica::reasoner::ASPSolverWrapper(ae, args);
		solverWrapper->init(solver);
		ae->addSolver(1, solverWrapper);
		alica::reasoner::ASPSolverWrapper* aspSolver = dynamic_cast<alica::reasoner::ASPSolverWrapper*>(ae->getSolver(1)); // "1" for ASPSolver

		aspSolver->getSolver()->loadFileFromConfig("assistanceBackgroundKnowledgeFile");
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

	alica::reasoner::ASPSolverWrapper* aspSolver = dynamic_cast<alica::reasoner::ASPSolverWrapper*>(ae->getSolver(1)); // "1" for ASPSolver


	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = "inconsistent(harryPotter1)";
	auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
	constraint->addRule(queryString);
	constraint->setType(::reasoner::ASPQueryType::Facts);
	constraint->setProgramSection("assistanceTestFacts");
	shared_ptr<::reasoner::ASPFactsQuery> queryObject = make_shared<::reasoner::ASPFactsQuery>(((::reasoner::ASPSolver*)(aspSolver->getSolver())), constraint);
	((::reasoner::ASPSolver*)(aspSolver->getSolver()))->registerQuery(queryObject);
	((::reasoner::ASPSolver*)(aspSolver->getSolver()))->ground( { {"assistanceBackground", {}}}, nullptr);
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

	EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The book harryPotter1 should be carried by more than one agent.";
	cout << queryObject->toString() << endl;
}

TEST_F(ASPDomainKnowledge, overloaded)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolverWrapper* aspSolver = dynamic_cast<alica::reasoner::ASPSolverWrapper*>(ae->getSolver(1)); // "1" for ASPSolver

	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = "overloaded(leonardo)";
	auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
	constraint->addRule(queryString);
	constraint->setType(::reasoner::ASPQueryType::Facts);
	constraint->setProgramSection("assistanceTestFacts");
	shared_ptr<::reasoner::ASPFactsQuery> queryObject = make_shared<::reasoner::ASPFactsQuery>(((::reasoner::ASPSolver*)(aspSolver->getSolver())), constraint);
	aspSolver->registerQuery(queryObject);
	((::reasoner::ASPSolver*)(aspSolver->getSolver()))->ground( { {"assistanceBackground", {}}}, nullptr);
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

	EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The agent can't carry by more than one thing.";
	cout << queryObject->toString() << endl;
}

TEST_F(ASPDomainKnowledge, largeObject)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolverWrapper* aspSolver = dynamic_cast<alica::reasoner::ASPSolverWrapper*>(ae->getSolver(1)); // "1" for ASPSolver
	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();

	string queryString = "overloaded(michelangelo)";
	auto constraint = make_shared<::reasoner::ASPCommonsTerm>();
	constraint->addRule(queryString);
	constraint->setProgramSection("assistanceTestFacts");
	constraint->setType(::reasoner::ASPQueryType::Facts);
	shared_ptr<::reasoner::ASPFactsQuery> queryObject = make_shared<::reasoner::ASPFactsQuery>(((::reasoner::ASPSolver*)(aspSolver->getSolver())), constraint);
	aspSolver->registerQuery(queryObject);
	((::reasoner::ASPSolver*)(aspSolver->getSolver()))->ground( { {"assistanceBackground", {}}}, nullptr);
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

	EXPECT_TRUE(queryObject->factsExistForAllModels()) << "The agent can't carry a large thing.";
	cout << queryObject->toString() << endl;
}
