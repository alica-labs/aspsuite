/*
 * AspQuery.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/AspQuery.h>
#include <alica_asp_solver/ASPSolver.h>
#include <clingo/clingocontrol.hh>

namespace alica
{
	namespace reasoner
	{

		AspQuery::AspQuery(ASPSolver* solver)
		{
			this->queryString = "empty";
			this->lifeTime = 1;
			this->satisfied = false;
			this->disjunction = false;
			this->solver = solver;
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString)
		{
			this->queryString = queryString;
			this->lifeTime = 1;
			this->satisfied = false;
			this->disjunction = false;
			this->solver = solver;
			this->queryValues = this->createQueryValues(queryString);
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString, int lifeTime)
		{
			this->queryString = queryString;
			this->lifeTime = lifeTime;
			this->satisfied = false;
			this->disjunction = false;
			this->solver = solver;
			this->queryValues = this->createQueryValues(queryString);
		}

		AspQuery::~AspQuery()
		{
		}

		vector<Gringo::ValVec> AspQuery::getCurrentModels()
		{
			return this->currentModels;
		}

		void AspQuery::setCurrentModels(vector<Gringo::ValVec> currentModels)
		{
			this->currentModels = currentModels;
		}

		int AspQuery::getLifeTime()
		{
			return this->lifeTime;
		}

		void AspQuery::setLifeTime(int lifeTime)
		{
			this->lifeTime = lifeTime;
		}

		string AspQuery::getQueryString()
		{
			return this->queryString;
		}

		bool AspQuery::setQueryString(string queryString)
		{
			if (this->queryString.compare("empty") == 0)
			{
				this->queryString = queryString;
				this->queryValues = this->createQueryValues(queryString);
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

		bool AspQuery::isSatisfied()
		{
			return this->satisfied;
		}

		void AspQuery::setSatisfied(bool satisfied)
		{
			this->satisfied = satisfied;
		}

		map<Gringo::Value, Gringo::ValVec> AspQuery::getSatisfiedPredicates()
		{
			return this->satisfiedPredicates;
		}

		void AspQuery::saveSatisfiedPredicate(Gringo::Value, Gringo::ValVec)
		{
		}

		bool AspQuery::isDisjunction()
		{
			return this->disjunction;
		}

		vector<Gringo::Value> AspQuery::getQueryValues()
		{
			return this->queryValues;
		}

		vector<Gringo::Value> AspQuery::createQueryValues(const std::string& queryString)
		{
			vector<Gringo::Value> ret;
			if (queryString.find(",") != string::npos && queryString.find(";") == string::npos)
			{
				size_t start = 0;
				size_t end = string::npos;
				string currentQuery = "";
				while (start != string::npos)
				{
					end = queryString.find(")", start);
					if (end == string::npos)
					{
						break;
					}
					currentQuery = queryString.substr(start, end - start + 1);
					currentQuery = supplementary::Configuration::trim(currentQuery);
					ret.push_back(this->solver->getGringoModule()->parseValue(currentQuery));
					start = queryString.find(",", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else if (queryString.find(";") != string::npos)
			{
				this->disjunction = true;
				size_t start = 0;
				size_t end = string::npos;
				string currentQuery = "";
				while (start != string::npos)
				{
					end = queryString.find(")", start);
					if (end == string::npos)
					{
						break;
					}
					currentQuery = queryString.substr(start, end - start + 1);
					currentQuery = supplementary::Configuration::trim(currentQuery);
					ret.push_back(this->solver->getGringoModule()->parseValue(currentQuery));
					start = queryString.find(";", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else
			{
				ret.push_back(this->solver->getGringoModule()->parseValue(queryString));
			}
			return ret;
		}

	} /* namespace reasoner */
} /* namespace alica */
