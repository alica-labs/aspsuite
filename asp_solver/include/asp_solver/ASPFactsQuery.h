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
		map<Gringo::Value, vector<Gringo::Value> > getFactModelMap();
		void setFactModelMap(map<Gringo::Value, vector<Gringo::Value> > factModelMap);
		shared_ptr<map<Gringo::Value, vector<Gringo::Value>>> getSatisfiedFacts();
	void saveSatisfiedFact(Gringo::Value key, Gringo::Value value);
	bool factsExistForAllModels();
	bool factsExistForAtLeastOneModel();
	void removeExternal();
	vector<pair<Gringo::Value, ASPTruthValue>> getASPTruthValues();

private:
	vector<Gringo::Value> queryValues;
	void createQueryValues(vector<string> queryString);
	// key := query value, value := predicates that satisfy the query value
	map<Gringo::Value, vector<Gringo::Value>> factModelMap;
};

}
/* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_ */
