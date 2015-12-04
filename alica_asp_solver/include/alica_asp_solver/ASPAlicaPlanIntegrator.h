/*
 * ASPAlicaPlanIntegrator.h
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPALICAPLANINTEGRATOR_H_
#define SRC_ASPALICAPLANINTEGRATOR_H_

#include <memory>
#include <ClingWrapper.h>

using namespace std;

namespace alica
{
	class Plan;

	namespace reasoner
	{

		class ASPAlicaPlanIntegrator
		{
		public:
			ASPAlicaPlanIntegrator(shared_ptr<supplementary::ClingWrapper> cw);
			virtual ~ASPAlicaPlanIntegrator();
			bool loadPlanTree(Plan* p);

		private:
			bool processPlan(Plan* p);

			shared_ptr<supplementary::ClingWrapper> cw;
			vector<long> processedPlanIds;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPALICAPLANINTEGRATOR_H_ */
