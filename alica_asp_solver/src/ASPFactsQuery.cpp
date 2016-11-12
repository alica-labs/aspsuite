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
			this->type = ASPQueryType::Facts;
			this->queryValues = this->createQueryValues(term->getRuleHead());
			for (auto value : this->queryValues)
			{
				this->headValues.emplace(value, vector<Gringo::Value>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			if (term->getProgrammSection().compare("") != 0)
			{
				auto loaded = this->solver->loadFileFromConfig(term->getProgrammSection());
#ifdef ASPSolver_DEBUG
				cout << "ASPSolver: Query contains rule: " << this->term->getRule() << endl;
#endif
				for (auto fact : this->term->getFacts())
				{
#ifdef ASPSolver_DEBUG
					cout << "ASPSolver: Query contains fact: " << fact << endl;
#endif
				}
				if (loaded)
				{
					this->solver->getClingo()->ground( { {term->getProgrammSection(), {}}}, nullptr);
				}
			}
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
					if (end == string::npos || end == queryString.size())
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

		bool ASPFactsQuery::factsExistForAtLeastOneModel()
		{
			for (auto queryValue : this->headValues)
			{
				if (queryValue.second.size() > 0)
				{
					return true;
				}
			}
			return false;
		}

		bool ASPFactsQuery::factsExistForAllModels()
		{
			for (auto queryValue : this->headValues)
			{
				if (queryValue.second.size() == 0)
				{
					return false;
				}
			}
			return true;
		}

		void ASPFactsQuery::removeExternal()
		{
		}

		vector<pair<Gringo::Value, ASPTruthValue> > ASPFactsQuery::getASPTruthValues()
		{
			vector<pair<Gringo::Value, ASPTruthValue>> ret;
			for(auto iter : this->getHeadValues())
			{
				if(iter.second.size() == 0)
				{
					ret.push_back(pair<Gringo::Value, ASPTruthValue>(iter.first, ASPTruthValue::Unknown));
				}
				else
				{
					if(iter.second.at(0).sign())
					{
						ret.push_back(pair<Gringo::Value, ASPTruthValue>(iter.first, ASPTruthValue::True));
					}
					else
					{
						ret.push_back(pair<Gringo::Value, ASPTruthValue>(iter.first, ASPTruthValue::False));
					}
				}

			}
			return ret;
		}

	} /* namespace reasoner */
} /* namespace alica */

