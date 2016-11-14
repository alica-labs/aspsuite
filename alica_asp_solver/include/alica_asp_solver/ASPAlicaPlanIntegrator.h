/*
 * ASPAlicaPlanIntegrator.h
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPALICAPLANINTEGRATOR_H_
#define SRC_ASPALICAPLANINTEGRATOR_H_

#include <memory>
#include <clingo/clingocontrol.hh>

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
			ASPAlicaPlanIntegrator(shared_ptr<ClingoLib> clingo, ASPGenerator* gen);
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
			shared_ptr<ClingoLib> clingo;

			vector<long> processedPlanIds;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPALICAPLANINTEGRATOR_H_ */
