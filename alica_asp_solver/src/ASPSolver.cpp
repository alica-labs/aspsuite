/*
 * ASPSolver.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#include "ASPSolver.h"
#include "engine/model/Plan.h"
#include <SystemConfig.h>

namespace alica
{
	namespace reasoner
	{

		ASPSolver::ASPSolver(AlicaEngine* ae) : IConstraintSolver(ae)
		{
			this->planIntegrator = new ASPAlicaPlanIntegrator();
			this->cw = make_shared<supplementary::ClingWrapper>();
			this->cw->setNoWarnings(true);

			// read alica background knowledge from static file
			supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
			string alicaBackGroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("alicaBackgroundKnowledgeFile", NULL);
			alicaBackGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),alicaBackGroundKnowledgeFile);
			cout <<"ASPSolver: " << alicaBackGroundKnowledgeFile << endl;
			this->cw->addKnowledgeFile(alicaBackGroundKnowledgeFile);

			this->cw->init();
			Gringo::SolveResult result = this->cw->solve();
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
			return nullptr;//make_shared<SolverVariable>();
		}

		bool ASPSolver::validatePlan(Plan* plan)
		{
			if (true)// Add plan only, if it is not already in background knowledge
			{
				this->planIntegrator->loadPlanTree(plan, cw);
			}
			// Ask for broken stuff related to the given plan

			// Present results

			return false;
		}

	} /* namespace reasoner */
} /* namespace alica */
