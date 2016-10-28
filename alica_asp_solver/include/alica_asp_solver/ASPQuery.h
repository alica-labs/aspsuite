/*
 * AspQuery.h
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#ifndef SYMROCK_ALICA_ASP_SOLVER_INCLUDE_ALICA_ASP_SOLVER_ASPQUERY_H_
#define SYMROCK_ALICA_ASP_SOLVER_INCLUDE_ALICA_ASP_SOLVER_ASPQUERY_H_

#include <string>
#include <clingo/clingocontrol.hh>
#include "alica_asp_solver/Term.h"

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class ASPSolver;
		class ASPQuery
		{
		public:
			ASPQuery(ASPSolver* solver, shared_ptr<alica::reasoner::Term> term);
			ASPQuery(ASPSolver* solver, string programmSection, int lifeTime = 1);
			ASPQuery(ASPSolver* solver, string queryString, string programmSection);
			ASPQuery(ASPSolver* solver, string queryString, string programmSection, int lifeTime = 1);
			virtual ~ASPQuery();

			shared_ptr<vector<Gringo::ValVec>> getCurrentModels();

			int getLifeTime();
			void setLifeTime(int lifeTime);
			void reduceLifeTime();

			ASPSolver* getSolver();
			string getProgrammSection();
			void setProgrammSection(string programmSection);
			string getQueryString();
			bool setQueryString(string queryString);
			vector<Gringo::Value> getQueryValues();
			bool isDisjunction();

			map<Gringo::Value, vector<Gringo::Value>> getPredicateModelMap();
			map<Gringo::Value, vector<Gringo::Value>> getRuleModelMap();
			map<Gringo::Value,vector<Gringo::Value>> getHeadValues();
			shared_ptr<map<Gringo::Value, vector<Gringo::Value>>> getSattisfiedRules();
			shared_ptr<map<Gringo::Value, vector<Gringo::Value>>> getSattisfiedPredicates();

			void saveRuleModelPair(Gringo::Value key, Gringo::Value value);
			void saveStaisfiedPredicate(Gringo::Value key, Gringo::Value value);
			void saveHeadValuePair(Gringo::Value key, Gringo::Value value);

			vector<string> getRules();
			void addRule(string pragrammSection, string rule, bool ground);

			void createHeadQueryValues(string queryString);

			string toString();

		private:
			/**
			 * queryString is used to ask the solver if specific predicates are true.
			 * predicates are separated by "," meaning all of them will be in the same rule and ";"
			 * meaning that there is a rule for every predicate
			 */
			string queryString;
			ASPSolver* solver;
			shared_ptr<vector<Gringo::ValVec>> currentModels;
			vector<Gringo::Value> queryValues;
			vector<string> rules;
			// key := query value, value := predicates that satisfy the query value
			map<Gringo::Value, vector<Gringo::Value>> predicateModelMap;
			// key := rule , value := model which satisfies query
			map<Gringo::Value, vector<Gringo::Value>> ruleModelMap;
			// key := headValue , value := values which satisfies it
			map<Gringo::Value, vector<Gringo::Value>> headValues;
			// lifeTime == 1 => query is used once
			// lifeTime == x => query is used x times
			// LifeTime == -1 => query is used util unregistered
			int lifeTime;
			bool disjunction;
			string programmSection;
			vector<Gringo::Value> createQueryValues(string queryString);
			void generateRules(string queryString);
			shared_ptr<alica::reasoner::Term> term;

		};

		} /* namespace reasoner */
	} /* namespace alica */

#endif /* SYMROCK_ALICA_ASP_SOLVER_INCLUDE_ALICA_ASP_SOLVER_ASPQUERY_H_ */
