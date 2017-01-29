/*
 * ASPAlicaPlanIntegrator.h
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVERWRAPPERPLANINTEGRATOR_H_
#define SRC_ASPSOLVERWRAPPERPLANINTEGRATOR_H_

#include <memory>
#include <asp_commons/IASPSolver.h>
using namespace std;

//#define ASPAlicaPlanIntegratorDebug

namespace alica
{
	class Plan;
	class PreCondition;
	class RuntimeCondition;
	class Condition;
	class State;
	class PlanType;

	namespace reasoner
	{

		class ASPGenerator;

		//! Integrates ALICA program into ASP.
		/**
		 * Helps to integrate all parts of an ALCIA program into the ASP solver.
		 */
		class ASPAlicaPlanIntegrator
		{
		public:
			ASPAlicaPlanIntegrator(::reasoner::IASPSolver* solver, ASPGenerator* gen);
			virtual ~ASPAlicaPlanIntegrator();
			void loadPlanTree(Plan* p);

		private:
			void processPlan(Plan* p, uint64_t instanceElementHash);
			void processPreCondition(PreCondition* cond);
			void processRuntimeCondition(RuntimeCondition* cond);
			uint64_t handleRunningPlan(Plan* rootPlan);
			uint64_t handleRunningPlan(Plan* childPlan, State* state, uint64_t instanceElementHash);
			uint64_t handleRunningPlan(Plan* childPlan, State* state, PlanType* planType, uint64_t instanceElementHash);
			void handleCondString(const string& condString, string prefix, Condition* cond);

			ASPGenerator* gen;
			::reasoner::IASPSolver* solver;

			vector<long> processedPlanIds;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVERWRAPPERPLANINTEGRATOR_H_ */
