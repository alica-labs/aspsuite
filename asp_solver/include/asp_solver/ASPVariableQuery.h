/*
 * ASPVariableQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ASP_SOLVER_ASPVARIABLEQUERY_H_
#define INCLUDE_ASP_SOLVER_ASPVARIABLEQUERY_H_

#include <clingo/clingocontrol.hh>

#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/ASPTerm.h>

//#define ASPVARIABLEQUERY_DEBUG

namespace reasoner
{

	class ASPVariableQuery : public ASPQuery
	{
	public:
		ASPVariableQuery(ASPSolver* solver, shared_ptr<reasoner::ASPTerm> term);
		virtual ~ASPVariableQuery();
		vector<string> getRules();
		ASPQueryType getType();
		void removeExternal();

	private:
		void createHeadQueryValues(vector<string> queryVec);
		string expandRule(string rule);
		string expandFact(string fact);
		shared_ptr<Gringo::Value> external;
		string queryProgramSection;
		string externalName;
		void createProgramSection();
	};

} /* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPVARIABLEQUERY_H_ */
