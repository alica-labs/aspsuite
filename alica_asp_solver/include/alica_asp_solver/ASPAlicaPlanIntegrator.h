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

namespace alica
{
	class Plan;
	class PreCondition;
	class RuntimeCondition;
	class Condition;
	class State;

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
			void processPlan(Plan* p);
			void processPreCondition(PreCondition* cond);
			void processRuntimeCondition(RuntimeCondition* cond);
			void handleRunningPlan(Plan* childPlan, State* state);
			void handleCondString(const string& condString, string prefix, Condition* cond);

			ASPGenerator* gen;
			shared_ptr<ClingoLib> clingo;

			vector<long> processedPlanIds;


			uint64_t instanceElementHash;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPALICAPLANINTEGRATOR_H_ */
