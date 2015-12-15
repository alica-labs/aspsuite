/*
 * ASPAlicaPlanIntegrator.cpp
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPAlicaPlanIntegrator.h"

#include "engine/model/Plan.h"
#include "engine/model/State.h"

using namespace std;

namespace alica
{
	namespace reasoner
	{

		ASPAlicaPlanIntegrator::ASPAlicaPlanIntegrator(shared_ptr<ClingoLib> clingo)
		{
			this->clingo = clingo;
		}

		ASPAlicaPlanIntegrator::~ASPAlicaPlanIntegrator()
		{
		}

		bool ASPAlicaPlanIntegrator::loadPlanTree(Plan* p)
		{
			this->processedPlanIds.clear();

			this->clingo->add("PlanBase", {}, "topLevelPlan(p" + std::to_string(p->getId()) + ").");
			bool hasTreeProperty = this->processPlan(p);
			this->clingo->ground( { {"PlanBase", {}}}, nullptr);
			this->clingo->solve(nullptr, {});
			return hasTreeProperty;
		}

		bool ASPAlicaPlanIntegrator::processPlan(Plan* p)
		{
			long currentPlanId = p->getId();
			if (find(processedPlanIds.begin(), processedPlanIds.end(), currentPlanId) != processedPlanIds.end())
			{// already processed, so there is a cycle
				cout << "PlanIntegrator: Cycle detected!" << endl;
				return false;
			}
			else
			{
				processedPlanIds.push_back(currentPlanId);
			}

			// flag to signal, if the plan tree really is a tree
			bool hasTreeProperty = true;

			// add the plan to the knowledge base.
			cout << "Plan Integrator: processing Plan " << p->getId() << endl;

			this->clingo->add("PlanBase", {}, "plan(p" + std::to_string(p->getId()) + ").");

			for (auto state : p->getStates())
			{
				for (auto abstractChildPlan : state->getPlans())
				{
					alica::Plan* childPlan = dynamic_cast<alica::Plan*>(abstractChildPlan);
					if (childPlan != nullptr)
					{
						if (!this->processPlan(childPlan))
						{
							cout << "PlanIntegrator: treeProperty violated!" << endl;
							hasTreeProperty = false;
						}
					}
				}
			}

			return hasTreeProperty;
		}

	} /* namespace reasoner */
} /* namespace alica */
