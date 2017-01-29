/*
 * ASPSolverWrapper.h
 *
 *  Created on: Jan 24, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_ASPSOLVERWRAPPER_H_
#define INCLUDE_ASPSOLVERWRAPPER_H_

#include <engine/constraintmodul/ISolver.h>
#include <engine/model/Variable.h>

#include <vector>
#include <string>
#include <memory>

using namespace std;

namespace reasoner
{
	class IASPSolver;
	class ASPCommonsTerm;
	class ASPCommonsVariable;
	class ASPQuery;
}

namespace alica
{
	class SolverVariable;
	class SolverTerm;
	class AlicaEngine;
	class Plan;
	namespace reasoner
	{
		class ASPAlicaPlanIntegrator;
		class ASPGenerator;
		class ASPSolverWrapper : public alica::ISolver
		{
		public:
			ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args);
			virtual ~ASPSolverWrapper();

			bool existsSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls);
			bool getSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);
			::reasoner::IASPSolver* getSolver();
			void init(::reasoner::IASPSolver* solver);
			int getQueryCounter();
			bool validatePlan(Plan* plan);
			alica::reasoner::ASPGenerator* gen;

			void removeDeadQueries();
			bool registerQuery(shared_ptr<::reasoner::ASPQuery> query);
			bool unregisterQuery(shared_ptr<::reasoner::ASPQuery> query);
			void printStats();
			vector<shared_ptr<::reasoner::ASPQuery>> getRegisteredQueries();

		private:
			AlicaEngine* ae;
			::reasoner::IASPSolver* solver;
			bool masterPlanLoaded;
			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
			void integrateRules();
			shared_ptr<::reasoner::ASPCommonsTerm> toCommonsTerm(shared_ptr<SolverTerm> term);
			shared_ptr<::reasoner::ASPCommonsVariable> toCommonsVariable(shared_ptr<SolverVariable> var);
		};
	}

} /* namespace alica */

#endif /* INCLUDE_ASPSOLVERWRAPPER_H_ */
