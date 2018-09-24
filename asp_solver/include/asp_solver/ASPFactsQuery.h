/*
 * ASPFactsQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#ifndef INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_
#define INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_

//#include <clingo/clingocontrol.hh>
#include <clingo.hh>

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
		map<Clingo::Symbol, vector<Clingo::Symbol> > getFactModelMap();
		void setFactModelMap(map<Clingo::Symbol, vector<Clingo::Symbol> > factModelMap);
		shared_ptr<map<Clingo::Symbol, vector<Clingo::Symbol>>> getSatisfiedFacts();
		void saveSatisfiedFact(Clingo::Symbol key, Clingo::Symbol value);
		bool factsExistForAllModels();
		bool factsExistForAtLeastOneModel();
		void removeExternal();
		vector<pair<Clingo::Symbol, ASPTruthValue>> getASPTruthValues();
		void onModel(Clingo::Model& clingoModel);

	private:
		vector<Clingo::Symbol> queryValues;
		void createQueryValues(vector<string> queryString);
		// key := query value, value := predicates that satisfy the query value
		map<Clingo::Symbol, vector<Clingo::Symbol>> factModelMap;
	};

}
/* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPFACTSQUERY_H_ */
