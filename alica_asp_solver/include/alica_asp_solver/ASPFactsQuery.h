/*
 * ASPFactsQuery.h
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_ASPFACTSQUERY_H_
#define INCLUDE_ALICA_ASP_SOLVER_ASPFACTSQUERY_H_

#include <alica_asp_solver/ASPQuery.h>
#include <alica_asp_solver/ASPQueryType.h>
#include <alica_asp_solver/ASPTruthValue.h>
#include <clingo/clingocontrol.hh>
#include "alica_asp_solver/ASPTerm.h"

namespace alica
{
	namespace reasoner
	{

		class ASPFactsQuery : public ASPQuery
		{
		public:
			ASPFactsQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term);
			virtual ~ASPFactsQuery();
			map<Gringo::Value, vector<Gringo::Value> > getFactModelMap();
			void setFactModelMap(map<Gringo::Value, vector<Gringo::Value> > factModelMap);
			shared_ptr<map<Gringo::Value, vector<Gringo::Value>>> getSatisfiedFacts();
			void saveSatisfiedFact(Gringo::Value key, Gringo::Value value);
			bool isTrueForAllModels();
			bool isTrueForAtLeastOneModel();
			void removeExternal();
			vector<pair<Gringo::Value, ASPTruthValue>> getASPTruthValues();

		private:
			const ASPQueryType type = ASPQueryType::Facts;
			vector<Gringo::Value> queryValues;
			vector<Gringo::Value> createQueryValues(string queryString);
			// key := query value, value := predicates that satisfy the query value
			map<Gringo::Value, vector<Gringo::Value>> factModelMap;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_ASPFACTSQUERY_H_ */
