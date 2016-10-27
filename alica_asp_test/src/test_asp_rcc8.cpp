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
#include <alica_asp_solver/AspQuery.h>
#include <alica_asp_solver/Term.h>

class ASPRCC8 : public ::testing::Test
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

		std::vector<char const *> args {"clingo", "-W", "no-atom-undefined", "--number=0", nullptr};

		start = std::chrono::high_resolution_clock::now(); // start time measurement
		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae, args));
		alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

		aspSolver->loadFromConfigIfNotYetLoaded("rcc8CompositionTableFile");
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

TEST_F(ASPRCC8, Department)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	aspSolver->loadFromConfigIfNotYetLoaded("rrc8DepartmentSectionFile");
	// start time measurement
	std::chrono::_V2::system_clock::time_point startGrounding = std::chrono::high_resolution_clock::now();
	aspSolver->ground( { {"department_sections", {}}}, nullptr);
	aspSolver->ground( { {"rcc8_composition_table", {}}}, nullptr);
	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - startGrounding).count() << " ms" << endl;

	string queryString = "externallyConnected(studentArea, mainHallA), disconnected(studentArea, mainHallB)";
	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString,"department_sections",
																								1);
	aspSolver->registerQuery(queryObject);
	aspSolver->ground( { {"department_sections", {}}}, nullptr);
	if (!aspSolver->solve())
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	cout << queryObject->toString() << endl;
	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject))
			<< "The studentArea should be externallyConnected to mainHallA and disconnected to mainHallB.";

}

TEST_F(ASPRCC8, DisjunctionInQuery)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	aspSolver->loadFromConfigIfNotYetLoaded("rrc8DepartmentSectionFile");
	// start time measurement
	std::chrono::_V2::system_clock::time_point startGrounding = std::chrono::high_resolution_clock::now();
	aspSolver->ground( { {"department_sections", {}}}, nullptr);
	aspSolver->ground( { {"rcc8_composition_table", {}}}, nullptr);
	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - startGrounding).count() << " ms" << endl;

	string queryString = "externallyConnected(studentArea, mainHallA); disconnected(studentArea, mainHallB)";
	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString, "department_sections",
																								1);
	queryObject->createHeadQueryValues("c(CountOfExCon)");
	queryObject->addRule("department_sections", "c(CountOfExCon) :- CountOfExCon = #count{S : externallyConnected(X, S)}.", true);
	aspSolver->registerQuery(queryObject);
	if (!aspSolver->solve())
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}
	aspSolver->removeDeadQueries();
	EXPECT_TRUE(queryObject->isDisjunction()) << "The query should be a disjunction.";

	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject))
			<< "The StudentArea should be externallyConnected to mainHallA) and disconnected to mainHallB).";

	EXPECT_TRUE(queryObject->getRules().size() == 3) << "The query should create 3 ruless but contains "
			<< queryObject->getRules().size() << ".";

	EXPECT_TRUE(queryObject->getLifeTime() == 0) << "The query should be expired but lifetime is:"
			<< queryObject->getLifeTime() << ".";

	EXPECT_TRUE(aspSolver->getRegisteredQueriesCount() == 0) << "There shouldn't be any query left but there are "
			<< aspSolver->getRegisteredQueriesCount() << " left.";

	cout << queryObject->toString() << endl;
}


//TODO this is a full test of the RCC8 composition table with 64 test facts which results in over 750000 models
//TEST_F(ASPRCC8, RCC8CompositionTable)
//{
//	alica::reasoner::Term term;
//	term.setRule("freeFairyTailBook(X) :- book(X), genre(fairyTail), bookGenre(X, fairyTail), agent(Y), not on(X,Y).");
//	cout << "Head: " << term.getRuleHead() << endl;
//	cout << "Body: " << term.getRuleBody() << endl;
//	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
//			<< "Unable to initialise the ALICA Engine!";
//
//	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
//
//	string rcc8TestFactsFile = (*sc)["ASPSolver"]->get<string>("rcc8TestFactsFile", NULL);
//	rcc8TestFactsFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
//																rcc8TestFactsFile);
//	cout << "ASPSolver: " << rcc8TestFactsFile << endl;
//	aspSolver->load(rcc8TestFactsFile);
//	// start time measurement
//	std::chrono::_V2::system_clock::time_point startGrounding = std::chrono::high_resolution_clock::now();
//	aspSolver->ground( { {"rcc8TestFacts", {}}}, nullptr);
//	aspSolver->ground( { {"rcc8_composition_table", {}}}, nullptr);
//	// stop time measurement and report
//	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
//	cout << "Measured Grounding Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - startGrounding).count() << " ms" << endl;
//
//	string queryString = "disconnected(b16,c16), disconnected(a14,c14)";
//	shared_ptr<alica::reasoner::AspQuery> queryObject = make_shared<alica::reasoner::AspQuery>(aspSolver, queryString,
//																								1);
//	queryObject->createRules();
//	aspSolver->registerQuery(queryObject);
//	if (!aspSolver->solve())
//	{
//		cout << "ASPAlicaTest: No Model found!" << endl;
//	}
//	else
//	{
//		aspSolver->printStats();
//	}
//
//	EXPECT_TRUE(aspSolver->isTrueForAllModels(queryObject))
//			<< "The StudentArea should be externallyConnected to mainHallA) and disconnected to mainHallB).";
//
//}


