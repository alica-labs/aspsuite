/*
 * test_cling_wrap.cpp
 *
 *  Created on: Jul 28, 2014
 *      Author: Stephan Opfer
 */

#include <gtest/gtest.h>
#include "ClingWrapper.h"
#include "External.h"
#include "BaseLiteral.h"
#include "BoolLiteral.h"
#include "clasp/solver.h"
#include <chrono>
#include <map>
#include <vector>
#include <memory>

using namespace std;

TEST(AspAlicaStaticFile, simpleTest)
{
	chrono::_V2::system_clock::time_point start = chrono::high_resolution_clock::now();

	shared_ptr<supplementary::ClingWrapper> cw = make_shared<supplementary::ClingWrapper>();
	cw->setNoWarnings(true);
	cw->addKnowledgeFile("data/alica/alica-background-knowledge.lp");
	cw->addKnowledgeFile("data/alica/alica-simple-test.lp");
	cw->init();
	Gringo::SolveResult result = cw->solve();
	std::chrono::_V2::system_clock::time_point end = chrono::high_resolution_clock::now();
	cout << "Measured Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

	if (Gringo::SolveResult::SAT == result)
	{
		cout << "Model Found" << endl;
	}

	ASSERT_TRUE(cw->query("brokenState",
					{	"s1"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenTransition",
					{	"t"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenSynchronisation",
					{	"synch"}))<< cw->toStringLastModel();

	ASSERT_TRUE(cw->query("brokenTerminalState",
					{	"ts1"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenTerminalState",
					{	"ts2"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenTerminalState",
					{	"ts3"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenTerminalState",
					{	"ts4"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenTerminalState",
					{	"ts5"}))<< cw->toStringLastModel();

	ASSERT_TRUE(cw->query("brokenPlanTaskPair",
					{	"p5", "task1"}))<< cw->toStringLastModel();

	ASSERT_TRUE(cw->query("brokenEntryPoint",
					{	"e1"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenEntryPoint",
					{	"e2"}))<< cw->toStringLastModel();
	ASSERT_TRUE(cw->query("brokenEntryPoint",
					{	"e3"}))<< cw->toStringLastModel();
}

TEST(AspAlicaStaticFile, treeShapedHierarchyTest)
{
	chrono::_V2::system_clock::time_point start = chrono::high_resolution_clock::now();

	supplementary::ClingWrapper cw;
	cw.addKnowledgeFile("data/alica/alica-background-knowledge.lp");
	cw.addKnowledgeFile("data/alica/alica-tree-shaped-hierarchy-test.lp");
	cw.init();
	Gringo::SolveResult result = cw.solve();
	std::chrono::_V2::system_clock::time_point end = chrono::high_resolution_clock::now();
	cout << "Measured Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

	if (Gringo::SolveResult::SAT == result)
	{
		cout << "Model Found" << endl;
	}

	ASSERT_TRUE(cw.query("brokenPlan",
					{	"p1"}))<< cw.toStringLastModel();
	ASSERT_TRUE(cw.query("brokenPlan",
					{	"p2"}))<< cw.toStringLastModel();
	ASSERT_TRUE(cw.query("brokenPlan",
					{	"p3"}))<< cw.toStringLastModel();
	ASSERT_TRUE(cw.query("brokenPlan",
					{	"p4"}))<< cw.toStringLastModel();
	ASSERT_TRUE(cw.query("brokenPlan",
					{	"p5"}))<< cw.toStringLastModel();

	//Debug Output
	auto queryResult = cw.queryAllTrue("brokenPlan", {"?"});
	for (auto trueBrokenPlan : *queryResult)
	{
		trueBrokenPlan.print(cout);
		cout << endl;
	}

	queryResult = cw.queryAllTrue("hasChildPlan", {"?", "?"});
	for (auto trueBrokenPlan : *queryResult)
	{
		trueBrokenPlan.print(cout);
		cout << endl;
	}
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

