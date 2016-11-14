/*
 * ASPVariableQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_
#define INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_

#include <alica_asp_solver/ASPQuery.h>
#include <alica_asp_solver/ASPQueryType.h>
#include <clingo/clingocontrol.hh>
#include "alica_asp_solver/ASPTerm.h"

//#define ASPVARIABLEQUERY_DEBUG

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

			void createHeadQueryValues(string queryString);
//			void addRule(string programSection, string rule, bool ground);
//			void addFact(string programSection, string fact, bool ground);
			ASPQueryType getType();
			void removeExternal();

		private:
			string expandRule(string rule);
			string expandFact(string fact);
			shared_ptr<Gringo::Value> external;
			string queryProgramSection;
			string externalName;
			void createProgramSection();
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLEQUERY_H_ */
