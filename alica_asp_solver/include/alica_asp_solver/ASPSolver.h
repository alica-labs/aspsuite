/*
 * ASPSolver.h
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVER_H_
#define SRC_ASPSOLVER_H_

#include "ASPAlicaPlanIntegrator.h"
#include <engine/constraintmodul/IConstraintSolver.h>
#include <clingo/clingocontrol.hh>
#include <SystemConfig.h>

#include <memory>
#include <vector>


namespace alica
{
	class AlicaEngine;
	class Plan;

	namespace reasoner
	{

		class ASPSolver : public alica::IConstraintSolver
		{
		public:
			ASPSolver(AlicaEngine* ae);
			virtual ~ASPSolver();

			bool existsSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls);
			bool getSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);
			bool validatePlan(Plan* plan);

			void disableWarnings(bool noWarns);
			void load(string filename);

		private:
			shared_ptr<ClingoLib> clingo;
			DefaultGringoModule gringoModule;
			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;

			bool checkMatchValues(const Gringo::Value* value1, const Gringo::Value* value2);
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVER_H_ */
