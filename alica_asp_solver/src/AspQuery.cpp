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
			this->solver = solver;
			this->disjunction = false;
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->counter = 0;
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString)
		{
			this->queryString = queryString;
			this->lifeTime = 1;
			this->solver = solver;
			this->disjunction = false;
			this->queryValues = this->createQueryValues(queryString);
			for (auto value : this->queryValues)
			{
				this->predicateModelMap.emplace(value, vector<Gringo::ValVec>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->counter = 0;
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString, int lifeTime)
		{
			this->queryString = queryString;
			this->lifeTime = lifeTime;
			this->solver = solver;
			this->disjunction = false;
			this->queryValues = this->createQueryValues(queryString);
			for (auto value : this->queryValues)
			{
				this->predicateModelMap.emplace(value, vector<Gringo::ValVec>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->counter = 0;
		}

		AspQuery::~AspQuery()
		{
		}

		shared_ptr<vector<Gringo::ValVec>> AspQuery::getCurrentModels()
		{
			return this->currentModels;
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
				for (auto value : this->queryValues)
				{
					this->predicateModelMap.emplace(value, vector<Gringo::ValVec>());
				}
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

		map<Gringo::Value, vector<Gringo::ValVec>> AspQuery::getPredicateModelMap()
		{
			return this->predicateModelMap;
		}

		void AspQuery::savePredicateModelPair(Gringo::Value key, Gringo::ValVec valueVector)
		{
			auto entry = this->predicateModelMap.find(key);
			if (entry != this->predicateModelMap.end())
			{
				entry->second.push_back(valueVector);
			}
		}

		bool AspQuery::isDisjunction()
		{
			return this->disjunction;
		}

		vector<Gringo::Value> AspQuery::getQueryValues()
		{
			return this->queryValues;
		}

		string AspQuery::toString()
		{
			stringstream ss;
			ss << "QueryString: " << this->queryString << "\n";
			ss << "Predicates with models: " << "\n";
			for (auto pair : this->predicateModelMap)
			{
				ss << "\tPredicate: " << pair.first << "\n";
				for (auto models : pair.second)
				{
					ss << "\t\t Model: ";
					for (auto predicate : models)
					{
						ss << predicate << " ";
					}
					ss << "\n";
				}
			}
			ss << (this->disjunction ? "Query is disjubction." : "Query is conjunction.") << "\n";
			ss << "Query will be used " << this->lifeTime << " times again.\n";
			ss << "Rules:" << "\n";
			for (auto rule : this->rules)
			{
				ss << "\t" << rule << "\n";
			}
			ss << "Rules with models:" << "\n";
			for (auto rule : this->ruleModelMap)
			{
				ss << "\tRule: " << rule.first << "\n";
				for (auto models : rule.second)
				{
					ss << "\t\t Model: ";
					for (auto predicate : models)
					{
						ss << predicate << " ";
					}
					ss << "\n";
				}
			}
			return ss.str();
		}

		shared_ptr<map<Gringo::Value, vector<Gringo::ValVec> > > AspQuery::getSattisfiedPredicates()
		{
			shared_ptr<map<Gringo::Value, vector<Gringo::ValVec> > > ret = make_shared<
					map<Gringo::Value, vector<Gringo::ValVec> > >();
			for (auto pair : this->predicateModelMap)
			{
				if (pair.second.size() > 0)
				{
					ret->emplace(pair);
				}
			}
			return ret;
		}

		void AspQuery::createRules(string domainName)
		{
			if (this->isDisjunction())
			{
				stringstream ss;
				string tmp = "";
				for (auto predicate : this->queryValues)
				{
					ss << "queryHolds(query" << this << counter << ")";
					tmp = ss.str();
					ss << " :- " << predicate << ".";
					this->addRule(domainName, ss.str(), tmp);
					counter++;
					ss.str("");
				}
			}
			else
			{
				stringstream ss;
				string tmp = "";
				ss << "queryHolds(query" << this << counter << ")";
				tmp = ss.str();
				ss << " :- " << this->queryString << ".";
				this->addRule(domainName, ss.str(), tmp);
				counter++;
				ss.str("");
			}
			this->solver->ground( { {domainName, {}}}, nullptr);
		}

		vector<string> AspQuery::getRules()
		{
			return this->rules;
		}

		void AspQuery::addRule(string domainName, string rule, string ruleIdentifier)
		{
			this->solver->getClingo()->add(domainName, {}, rule);
			this->rules.push_back(rule);

			this->ruleModelMap.emplace(this->solver->getGringoModule()->parseValue(ruleIdentifier),
										vector<Gringo::ValVec>());
		}

		void AspQuery::addRuleAndGround(string domainName, string rule)
		{
			stringstream ss;
			string tmp = "";
			ss << "queryHolds(query" << this << counter << ")";
			tmp = ss.str();
			counter++;
			rule = tmp + ", " + rule;
			this->solver->getClingo()->add(domainName, {}, rule);
			this->ruleModelMap.emplace(this->solver->getGringoModule()->parseValue(tmp), vector<Gringo::ValVec>());
			this->rules.push_back(rule);
			this->solver->ground( { {domainName, {}}}, nullptr);
		}

		map<Gringo::Value, vector<Gringo::ValVec> > AspQuery::getRuleModelMap()
		{
			return this->ruleModelMap;
		}

		shared_ptr<map<Gringo::Value, vector<Gringo::ValVec> > > AspQuery::getSattisfiedRules()
		{
			shared_ptr<map<Gringo::Value, vector<Gringo::ValVec> > > ret = make_shared<
					map<Gringo::Value, vector<Gringo::ValVec> > >();
			for (auto pair : this->ruleModelMap)
			{
				if (pair.second.size() > 0)
				{
					ret->emplace(pair);
				}
			}
			return ret;
		}

		void AspQuery::saveRuleModelPair(Gringo::Value key, Gringo::ValVec valueVector)
		{
			auto entry = this->ruleModelMap.find(key);
			if (entry != this->ruleModelMap.end())
			{
				entry->second.push_back(valueVector);
			}
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

