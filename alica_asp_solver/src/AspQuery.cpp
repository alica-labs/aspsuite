/*
 * AspQuery.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/AspQuery.h>

namespace alica
{
	namespace reasoner
	{

		AspQuery::AspQuery()
		{
			this->queryString = "empty";
			this->lifeTime = 1;
		}

		AspQuery::AspQuery(string queryString)
		{
			this->queryString = queryString;
			this->lifeTime = 1;
		}

		AspQuery::AspQuery(string queryString, int lifeTime)
		{
			this->queryString = queryString;
			this->lifeTime = lifeTime;
		}

		AspQuery::~AspQuery()
		{
		}

		vector<Gringo::ValVec> AspQuery::getCurrentModels()
		{
			return currentModels;
		}

		void AspQuery::setCurrentModels(vector<Gringo::ValVec> currentModels)
		{
			this->currentModels = currentModels;
		}

		int AspQuery::getLifeTime()
		{
			return lifeTime;
		}

		void AspQuery::setLifeTime(int lifeTime)
		{
				this->lifeTime = lifeTime;
		}

		string AspQuery::getQueryString()
		{
			return queryString;
		}

		bool AspQuery::setQueryString(string queryString)
		{
			if (this->queryString.compare("empty") == 0)
			{
				this->queryString = queryString;
				return true;
			}
			return false;
		}

		void AspQuery::reduceLifeTime()
		{
			if (this->lifeTime > 0)
			{
				this->lifeTime--;
			}
		}

	} /* namespace reasoner */
} /* namespace alica */
