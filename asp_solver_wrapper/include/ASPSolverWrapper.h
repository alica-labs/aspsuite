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

using namespace std;

namespace alica
{
	class AlicaEngine;
	namespace reasoner
	{
		class ASPSolverWrapper : public alica::ISolver
		{
		public:
			static const void* const WILDCARD_POINTER;
			static const string WILDCARD_STRING;

			ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args);
			virtual ~ASPSolverWrapper();

			bool existsSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls);
			bool getSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);

		private:
			AlicaEngine* ae;
		};
	}

} /* namespace alica */

#endif /* INCLUDE_ASPSOLVERWRAPPER_H_ */
