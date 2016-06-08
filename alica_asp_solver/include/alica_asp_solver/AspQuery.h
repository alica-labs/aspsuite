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

		class AspQuery
		{
		public:
			AspQuery();
			AspQuery(string queryString);
			AspQuery(string queryString, int lifeTime);
			virtual ~AspQuery();
			vector<Gringo::ValVec> getCurrentModels();
			void setCurrentModels(vector<Gringo::ValVec> currentModels);
			int getLifeTime();
			void setLifeTime(int lifeTime);
			string getQueryString();
			bool setQueryString(string queryString);
			void reduceLifeTime();

		private:
			string queryString;
			vector<Gringo::ValVec> currentModels;
			// lifeTime == 1 => query is used once
			// lifeTime == x => query is used x times
			// LifeTime == -1 => query is used util unregistered
			int lifeTime;

		};

		} /* namespace reasoner */
	} /* namespace alica */

#endif /* SYMROCK_ALICA_ASP_SOLVER_INCLUDE_ALICA_ASP_SOLVER_ASPQUERY_H_ */
