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

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class ASPSolver;
		class AspQuery
		{
		public:
			AspQuery(ASPSolver* solver);
			AspQuery(ASPSolver* solver, string queryString);
			AspQuery(ASPSolver* solver, string queryString, int lifeTime);
			virtual ~AspQuery();
			vector<Gringo::ValVec> getCurrentModels();
			void setCurrentModels(vector<Gringo::ValVec> currentModels);
			int getLifeTime();
			void setLifeTime(int lifeTime);
			string getQueryString();
			bool setQueryString(string queryString);
			void reduceLifeTime();
			bool isSatisfied();
			void setSatisfied(bool satisfied);
			map<Gringo::Value, Gringo::ValVec> getSatisfiedPredicates();
			void saveSatisfiedPredicate(Gringo::Value, Gringo::ValVec);
			bool isDisjunction();
			vector<Gringo::Value> getQueryValues();

		private:
			string queryString;
			ASPSolver* solver;
			vector<Gringo::ValVec> currentModels;
			vector<Gringo::Value> queryValues;
			// key=query value, value= model which satisfies query
			map<Gringo::Value, Gringo::ValVec> satisfiedPredicates;
			// lifeTime == 1 => query is used once
			// lifeTime == x => query is used x times
			// LifeTime == -1 => query is used util unregistered
			int lifeTime;
			bool satisfied;
			bool disjunction;
			vector<Gringo::Value> createQueryValues(std::string const &queryString);

		};

		} /* namespace reasoner */
	} /* namespace alica */

#endif /* SYMROCK_ALICA_ASP_SOLVER_INCLUDE_ALICA_ASP_SOLVER_ASPQUERY_H_ */
