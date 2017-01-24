/*
 * ASPSolverWrapper.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: stefan
 */

#include "../include/ASPSolverWrapper.h"

#include <engine/constraintmodul/ProblemDescriptor.h>
#include <engine/AlicaEngine.h>

namespace alica
{
	namespace reasoner
	{
		ASPSolverWrapper::ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args) :
				alica::ISolver(ae)
		{
			this->ae = ae;
		}

		ASPSolverWrapper::~ASPSolverWrapper()
		{
			// TODO Auto-generated destructor stub
		}

		bool ASPSolverWrapper::existsSolution(vector<alica::Variable*>& vars,
												vector<shared_ptr<ProblemDescriptor> >& calls)
		{
			//TODO call solver via asp common
		}

		bool ASPSolverWrapper::getSolution(vector<alica::Variable*>& vars,
											vector<shared_ptr<ProblemDescriptor> >& calls, vector<void*>& results)
		{
			//TODO call solver via asp common
		}

		shared_ptr<SolverVariable> ASPSolverWrapper::createVariable(long id)
		{
			//TODO call solver via asp common
		}

	}

} /* namespace alica */
