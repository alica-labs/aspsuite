/*
 * ASPFactsQuery.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#include <alica_asp_solver/ASPFactsQuery.h>
#include <alica_asp_solver/ASPSolver.h>

namespace alica
{
	namespace reasoner
	{

		ASPFactsQuery::ASPFactsQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term) :
				ASPQuery(solver, term)
		{
			this->solver = solver;
			this->queryValues = this->createQueryValues(term->getRule());
			for (auto value : this->queryValues)
			{
				this->factModelMap.emplace(value, vector<Gringo::Value>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
		}

		ASPFactsQuery::~ASPFactsQuery()
		{
		}

		vector<Gringo::Value> ASPFactsQuery::createQueryValues(string queryString)
		{
			vector<Gringo::Value> ret;
			if (queryString.compare("") == 0)
			{
				return ret;
			}
			if (queryString.find(",") != string::npos)
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
			else
			{
				ret.push_back(this->solver->getGringoModule()->parseValue(queryString));
			}
			return ret;
		}

		map<Gringo::Value, vector<Gringo::Value> > ASPFactsQuery::getFactModelMap()
		{
			return this->factModelMap;
		}

		void ASPFactsQuery::setFactModelMap(map<Gringo::Value, vector<Gringo::Value> > factModelMap)
		{
			this->factModelMap = factModelMap;
		}

		void ASPFactsQuery::saveSatisfiedFact(Gringo::Value key, Gringo::Value value)
		{
			auto entry = this->factModelMap.find(key);
			if (entry != this->factModelMap.end())
			{
				entry->second.push_back(value);
			}
		}

		shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > ASPFactsQuery::getSatisfiedFacts()
		{
			shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > ret = make_shared<
					map<Gringo::Value, vector<Gringo::Value> > >();
			for (auto pair : this->factModelMap)
			{
				if (pair.second.size() > 0)
				{
					ret->emplace(pair);
				}
			}
			return ret;
		}

	} /* namespace reasoner */
} /* namespace alica */
