/*
 * ASPSolver.h
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVER_H_
#define SRC_ASPSOLVER_H_

#include <engine/constraintmodul/IConstraintSolver.h>
#include <ClingWrapper.h>
#include <memory>
#include <vector>

namespace alica
{
	class AlicaEngine;
	class Plan;

	namespace reasoner
	{

		class ASPSolver : public IConstraintSolver
		{
		public:
			ASPSolver(AlicaEngine* ae);
			virtual ~ASPSolver();

			bool existsSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls);
			bool getSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);
			bool validatePlan(Plan* plan);

		private:
			shared_ptr<supplementary::ClingWrapper> cw;
			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVER_H_ */
