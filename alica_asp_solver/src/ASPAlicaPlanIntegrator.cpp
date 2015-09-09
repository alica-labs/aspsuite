/*
 * ASPAlicaPlanIntegrator.cpp
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#include "ASPAlicaPlanIntegrator.h"

#include "engine/model/Plan.h"

namespace alica
{
	namespace reasoner
	{

		ASPAlicaPlanIntegrator::ASPAlicaPlanIntegrator(shared_ptr<supplementary::ClingWrapper> cw)
		{
			this->cw = cw;
		}

		ASPAlicaPlanIntegrator::~ASPAlicaPlanIntegrator()
		{
			// TODO Auto-generated destructor stub
		}

		void ASPAlicaPlanIntegrator::loadPlanTree(Plan* p, )
		{
			this->processedPlanIds.clear();

			bool cylceDetected = this->processPlan(p);
		}

		bool ASPAlicaPlanIntegrator::processPlan(Plan* p)
		{
			long currentPlanId = p->getId();
			if (find(processedPlanIds.begin(), processedPlanIds.end(), currentPlanId) != processedPlanIds.end())
			{// already processed, so there is a cycle
				return false;
			}
			else
			{
				processedPlanIds.push_back(currentPlanId);
			}

			// flag to signal, if the plan tree really is a tree
			bool treeProperty = true;

			this->cw->

			return treeProperty;
		}

	} /* namespace reasoner */
} /* namespace alica */
