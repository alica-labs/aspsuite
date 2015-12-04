/*
 * ASPSolver.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPSolver.h"
#include "engine/model/Plan.h"
#include <SystemConfig.h>

namespace alica
{
	namespace reasoner
	{

		ASPSolver::ASPSolver(AlicaEngine* ae) : IConstraintSolver(ae)
		{
			this->cw = make_shared<supplementary::ClingWrapper>();
			this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(cw);
			this->cw->setNoWarnings(true);

			// read alica background knowledge from static file
			supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
			string alicaBackGroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("alicaBackgroundKnowledgeFile", NULL);
			alicaBackGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),alicaBackGroundKnowledgeFile);
			cout <<"ASPSolver: " << alicaBackGroundKnowledgeFile << endl;
			this->cw->addKnowledgeFile(alicaBackGroundKnowledgeFile);

			this->cw->init();
			//Gringo::SolveResult result = this->cw->solve();
		}

		ASPSolver::~ASPSolver()
		{
			// TODO Auto-generated destructor stub
		}

		bool ASPSolver::existsSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor> >& calls)
		{
			return false;
		}

		bool ASPSolver::getSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor> >& calls,
									vector<void*>& results)
		{
			return false;
		}

		shared_ptr<SolverVariable> ASPSolver::createVariable(long id)
		{
			// TODO
			return nullptr;//make_shared<SolverVariable>();
		}

		/**
		 * Validates the well-formedness of a given plan.
		 */
		bool ASPSolver::validatePlan(Plan* plan)
		{

			return this->planIntegrator->loadPlanTree(plan);

			// Ask for broken stuff related to the given plan

			// Present results
		}

	} /* namespace reasoner */
} /* namespace alica */
