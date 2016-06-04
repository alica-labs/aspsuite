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

class ASPRCC8 : public ::testing::Test
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

		std::vector<char const *> args {"clingo", "-W", "no-atom-undefined", "--number=0", nullptr};

		// "1" stands for the ASPSolver in this test suite only!
		ae->addSolver(1, new alica::reasoner::ASPSolver(ae, args));
		alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

		supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
		string rcc8CompositionTableFile = (*sc)["ASPSolver"]->get<string>("rcc8CompositionTableFile",
		NULL);
		rcc8CompositionTableFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																			rcc8CompositionTableFile);
		cout << "ASPSolver: " << rcc8CompositionTableFile << endl;
		aspSolver->load(rcc8CompositionTableFile);
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

TEST_F(ASPRCC8, multipleObjectCarry)
{
	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "ReusePlanWithoutCycle", "CarryBookMaster", ".", false))
			<< "Unable to initialise the ALICA Engine!";

	alica::reasoner::ASPSolver* aspSolver = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver

	string rcc8TestFactsFile = (*sc)["ASPSolver"]->get<string>("rcc8TestFactsFile", NULL);
	rcc8TestFactsFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(), rcc8TestFactsFile);
	cout << "ASPSolver: " << rcc8TestFactsFile << endl;
	aspSolver->load(rcc8TestFactsFile);
	aspSolver->ground( { {"rcc8TestFacts", {}}}, nullptr);
	aspSolver->ground( { {"rcc8_composition_table", {}}}, nullptr);

	// start time measurement
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	//string queryString = "disconnected(a, c) | externallyConnected(a, c) | partialOverlapping(a, c) | tangentialProperPart(a, c) | nonTangentialProperPart(a, c) | inverseTangentialProperPart(a, c) | inverseNonTangentialProperPart(a, c) | equal(a, c)";

	string queryString = "disconnected(a,b), disconnected(b,c), partialOverlapping(a,c)aaaa";//"disconnected(a, c)";
	aspSolver->registerQuery(queryString);

	if (!aspSolver->solve())
	{
		cout << "ASPAlicaTest: No Model found!" << endl;
	}
	else
	{
		aspSolver->printStats();
	}

	EXPECT_TRUE(aspSolver->isTrueForAtLeastOneModel(queryString)) << "The book harryPotter1 should be carried by more than one agent.";

	// stop time measurement and report
	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
}
