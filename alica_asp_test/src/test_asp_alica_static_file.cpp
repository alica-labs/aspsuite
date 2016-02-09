///*
// * test_cling_wrap.cpp
// *
// *  Created on: Jul 28, 2014
// *      Author: Stephan Opfer
// */
//
#include <gtest/gtest.h>

#include <SystemConfig.h>
#include <chrono>

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

// ALICA ASP Solver
#include <alica_asp_solver/ASPSolver.h>

class AspAlicaStaticTest : public ::testing::Test
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
//TEST_F(AspAlicaStaticTest, simpleTest)
//{
//	EXPECT_TRUE(ae->init(bc, cc, uc, crc, "Roleset", "MasterPlan", ".", false))
//			<< "Unable to initialise the Alica Engine!";
//
//	alica::reasoner::ASPSolver* clingo = dynamic_cast<alica::reasoner::ASPSolver*>(ae->getSolver(1)); // "1" for ASPSolver
//	alica::Plan* plan = ae->getPlanBase()->getMasterPlan();
//
//	EXPECT_TRUE(clingo->validatePlan(plan)) << "MasterPlan '" << plan->getName() << "' is invalid!";
//
//	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
//
//	clingo->disableWarnings(true);
//	clingo->load("src/etc/asp_background_knowledge/alica-background-knowledge.lp");
//	clingo->load("data/alica/alica-simple-test.lp");
//	clingo->ground({ {"", {}}}, nullptr);
//	// TODO: implement according to Alicas interface
//	Gringo::SolveResult result;// = clingo->getSolution();
//	std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
//	cout << "Measured Time: " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
//
//	if (Gringo::SolveResult::SAT == result)
//	{
//		cout << "Model Found" << endl;
//	}

//	ASSERT_TRUE(clingo->query("brokenState",
//					{	"s1"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenTransition",
//					{	"t"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenSynchronisation",
//					{	"synch"}))<< clingo->toStringLastModel();
//
//	ASSERT_TRUE(clingo->query("brokenTerminalState",
//					{	"ts1"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenTerminalState",
//					{	"ts2"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenTerminalState",
//					{	"ts3"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenTerminalState",
//					{	"ts4"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenTerminalState",
//					{	"ts5"}))<< clingo->toStringLastModel();
//
//	ASSERT_TRUE(clingo->query("brokenPlanTaskPair",
//					{	"p5", "task1"}))<< clingo->toStringLastModel();
//
//	ASSERT_TRUE(clingo->query("brokenEntryPoint",
//					{	"e1"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenEntryPoint",
//					{	"e2"}))<< clingo->toStringLastModel();
//	ASSERT_TRUE(clingo->query("brokenEntryPoint",
//					{	"e3"}))<< clingo->toStringLastModel();

//}

//TEST(AspAlicaStaticFile, treeShapedHierarchyTest)
//{
//	chrono::_V2::system_clock::time_point start = chrono::high_resolution_clock::now();
//
//	supplementary::ClingWrapper clingo;
//	clingo.addKnowledgeFile("src/etc/asp_background_knowledge/alica-background-knowledge.lp");
//	clingo.addKnowledgeFile("data/alica/alica-tree-shaped-hierarchy-test.lp");
//	clingo.init();
//	Gringo::SolveResult result = clingo.solve();
//	std::chrono::_V2::system_clock::time_point end = chrono::high_resolution_clock::now();
//	cout << "Measured Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
//
//	if (Gringo::SolveResult::SAT == result)
//	{
//		cout << "Model Found" << endl;
//	}
//
//	ASSERT_TRUE(clingo.query("brokenPlan",
//					{	"p1"}))<< clingo.toStringLastModel();
//	ASSERT_TRUE(clingo.query("brokenPlan",
//					{	"p2"}))<< clingo.toStringLastModel();
//	ASSERT_TRUE(clingo.query("brokenPlan",
//					{	"p3"}))<< clingo.toStringLastModel();
//	ASSERT_TRUE(clingo.query("brokenPlan",
//					{	"p4"}))<< clingo.toStringLastModel();
//	ASSERT_TRUE(clingo.query("brokenPlan",
//					{	"p5"}))<< clingo.toStringLastModel();
//
//	//Debug Output
//	auto queryResult = clingo.queryAllTrue("brokenPlan", {"?"});
//	for (auto trueBrokenPlan : *queryResult)
//	{
//		trueBrokenPlan.print(cout);
//		cout << endl;
//	}
//
//	queryResult = clingo.queryAllTrue("hasChildPlan", {"?", "?"});
//	for (auto trueBrokenPlan : *queryResult)
//	{
//		trueBrokenPlan.print(cout);
//		cout << endl;
//	}
//}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

