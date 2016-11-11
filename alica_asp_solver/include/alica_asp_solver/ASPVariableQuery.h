/*
 * ASPVariableQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_
#define INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_

#include <alica_asp_solver/ASPQuery.h>
#include <alica_asp_solver/ASPQueryType.h>
#include <clingo/clingocontrol.hh>
#include "alica_asp_solver/ASPTerm.h"

namespace alica
{
	namespace reasoner
	{

		class ASPVariableQuery : public ASPQuery
		{
		public:
			ASPVariableQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term);
			virtual ~ASPVariableQuery();

			vector<string> getRules();
			void addRule(string pragrammSection, string rule, bool ground);

			void createHeadQueryValues(string queryString);

		private:
			const ASPQueryType type = ASPQueryType::Variable;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_ */
