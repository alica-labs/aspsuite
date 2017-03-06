/*
 * ASPFactsQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#ifndef INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_
#define INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_

#include <clingo/clingocontrol.hh>

#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPTruthValue.h>

namespace reasoner
{

	class ASPSolver;
	class ASPFactsQuery : public ASPQuery
	{
	public:
		ASPFactsQuery(ASPSolver* solver, shared_ptr<ASPCommonsTerm> term);
		virtual ~ASPFactsQuery();
		map<Gringo::Symbol, vector<Gringo::Symbol> > getFactModelMap();
		void setFactModelMap(map<Gringo::Symbol, vector<Gringo::Symbol> > factModelMap);
		shared_ptr<map<Gringo::Symbol, vector<Gringo::Symbol>>> getSatisfiedFacts();
		void saveSatisfiedFact(Gringo::Symbol key, Gringo::Symbol value);
		bool factsExistForAllModels();
		bool factsExistForAtLeastOneModel();
		void removeExternal();
		vector<pair<Gringo::Symbol, ASPTruthValue>> getASPTruthValues();

private:
	vector<Gringo::Symbol> queryValues;
	void createQueryValues(vector<string> queryString);
	// key := query value, value := predicates that satisfy the query value
	map<Gringo::Symbol, vector<Gringo::Symbol>> factModelMap;
};

}
/* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_ */
