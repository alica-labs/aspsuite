#pragma once

#include <engine/constraintmodul/ISolver.h>
#include <engine/model/Variable.h>
#include <asp_commons/AnnotatedValVec.h>

#include <vector>
#include <string>
#include <memory>

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
		class ASPSolverWrapper : public alica::ISolver<ASPSolverWrapper, ::reasoner::AnnotatedValVec>
		{
		public:
			ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args);
			virtual ~ASPSolverWrapper();

			bool existsSolutionImpl(SolverContext* ctx, std::vector<std::shared_ptr<ProblemDescriptor>>& calls);
			bool getSolutionImpl(SolverContext* ctx, std::vector<std::shared_ptr<ProblemDescriptor>>& calls,
									 std::vector<::reasoner::AnnotatedValVec>& results);
            std::shared_ptr<SolverVariable> createVariable(uint64_t id);
			::reasoner::IASPSolver* getSolver();
			void init(::reasoner::IASPSolver* solver);
			int getQueryCounter();
			bool validatePlan(Plan* plan);
			alica::reasoner::ASPGenerator* gen;

			void removeDeadQueries();
			bool registerQuery(std::shared_ptr<::reasoner::ASPQuery> query);
			bool unregisterQuery(std::shared_ptr<::reasoner::ASPQuery> query);
			void printStats();
            std::vector<std::shared_ptr<::reasoner::ASPQuery>> getRegisteredQueries();

		private:
			AlicaEngine* ae;
			::reasoner::IASPSolver* solver;
			bool masterPlanLoaded;
            std::shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
			void integrateRules();
			std::shared_ptr<::reasoner::ASPCommonsTerm> toCommonsTerm(std::shared_ptr<SolverTerm> term);
            std::shared_ptr<::reasoner::ASPCommonsVariable> toCommonsVariable(std::shared_ptr<SolverVariable> var);
		};
	}

} /* namespace alica */
