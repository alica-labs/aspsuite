/*
 * AspQuery.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/AspQuery.h>
#include <alica_asp_solver/ASPSolver.h>
#include <clingo/clingocontrol.hh>
#include <regex>

namespace alica
{
	namespace reasoner
	{

		AspQuery::AspQuery(ASPSolver* solver, shared_ptr<alica::reasoner::Term> term)
		{
			this->queryString = "";
			this->solver = solver;
			this->term = term;
			this->programmSection = term->getProgrammSection();
			this->lifeTime = term->getLifeTime();
			this->disjunction = false;
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			auto loaded = this->solver->loadFromConfigIfNotYetLoaded(this->term->getProgrammSection());
			this->createHeadQueryValues(this->term->getRuleHead());
#ifdef ASPSolver_DEBUG
			cout << "ASPSolver: Query contains rule: " << this->term->getRule() << endl;
#endif
			this->addRule(this->term->getProgrammSection(), this->term->getRule(), false);
			for (auto fact : this->term->getFacts())
			{
#ifdef ASPSolver_DEBUG
				cout << "ASPSolver: Query contains fact: " << fact << endl;
#endif
				this->addRule(this->term->getProgrammSection(), fact, false);
			}
			if (loaded)
			{
				this->solver->getClingo()->ground( { {this->term->getProgrammSection(), {}}}, nullptr);
			}
		}

		AspQuery::AspQuery(ASPSolver* solver, string pragrammSection, int lifeTime)
		{
			this->queryString = "";
			this->programmSection = pragrammSection;
			this->lifeTime = lifeTime;
			this->solver = solver;
			this->disjunction = false;
			for (auto value : this->queryValues)
			{
				this->predicateModelMap.emplace(value, vector<Gringo::Value>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->term = nullptr;
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString, string pragrammSection)
		{
			this->queryString = queryString;
			this->programmSection = pragrammSection;
			this->lifeTime = 1;
			this->solver = solver;
			this->disjunction = false;
			this->queryValues = this->createQueryValues(queryString);
			for (auto value : this->queryValues)
			{
				this->predicateModelMap.emplace(value, vector<Gringo::Value>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->term = nullptr;
		}

		AspQuery::AspQuery(ASPSolver* solver, string queryString, string pragrammSection, int lifeTime)
		{
			this->queryString = queryString;
			this->programmSection = pragrammSection;
			this->lifeTime = lifeTime;
			this->solver = solver;
			this->disjunction = false;
			this->queryValues = this->createQueryValues(queryString);
			for (auto value : this->queryValues)
			{
				this->predicateModelMap.emplace(value, vector<Gringo::Value>());
			}
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
			this->term = nullptr;
		}

		AspQuery::~AspQuery()
		{
		}

		void AspQuery::reduceLifeTime()
		{
			if (this->lifeTime > 0)
			{
				this->lifeTime--;
			}
		}

		void AspQuery::saveStaisfiedPredicate(Gringo::Value key, Gringo::Value value)
		{
			auto entry = this->predicateModelMap.find(key);
			if (entry != this->predicateModelMap.end())
			{
				entry->second.push_back(value);
			}
		}

		shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > AspQuery::getSattisfiedPredicates()
		{
			shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > ret = make_shared<
					map<Gringo::Value, vector<Gringo::Value> > >();
			for (auto pair : this->predicateModelMap)
			{
				if (pair.second.size() > 0)
				{
					ret->emplace(pair);
				}
			}
			return ret;
		}

		void AspQuery::addRule(string domainName, string rule, bool ground)
		{
			this->solver->getClingo()->add(domainName, {}, rule);
			this->rules.push_back(rule);
			if (ground)
			{
				this->solver->ground( { {domainName, {}}}, nullptr);
			}
		}

		shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > AspQuery::getSattisfiedRules()
		{
			shared_ptr<map<Gringo::Value, vector<Gringo::Value> > > ret = make_shared<
					map<Gringo::Value, vector<Gringo::Value> > >();
			for (auto pair : this->ruleModelMap)
			{
				if (pair.second.size() > 0)
				{
					ret->emplace(pair);
				}
			}
			return ret;
		}

		void AspQuery::saveRuleModelPair(Gringo::Value key, Gringo::Value value)
		{
			auto entry = this->ruleModelMap.find(key);
			if (entry != this->ruleModelMap.end())
			{
				entry->second.push_back(value);
			}
		}

		void AspQuery::saveHeadValuePair(Gringo::Value key, Gringo::Value value)
		{
			auto entry = this->headValues.find(key);
			if (entry != this->headValues.end())
			{
				entry->second.push_back(value);
			}
		}

		void AspQuery::generateRules(string queryString)
		{
			stringstream ss;
			string tmp = "";
			string rule = "";
			ss << "queryHolds(query" << this->solver->getQueryCounter() << ")";
			tmp = ss.str();
			rule = tmp + " :- " + queryString + ".";
			if (queryString.find(ASPSolver::WILDCARD_STRING) != string::npos)
			{
				int pos = rule.find(ASPSolver::WILDCARD_STRING);
				rule.replace(pos, ASPSolver::WILDCARD_STRING.length(), "X");
			}
			this->solver->getClingo()->add(this->programmSection, {}, rule);
			this->ruleModelMap.emplace(this->solver->getGringoModule()->parseValue(tmp), vector<Gringo::Value>());
			this->rules.push_back(rule);

			cout << rule << endl;

		}

		void AspQuery::createHeadQueryValues(string queryString)
		{
			vector<string> valuesToParse;
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
					valuesToParse.push_back(currentQuery);
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
					valuesToParse.push_back(currentQuery);
					start = queryString.find(";", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else
			{
				valuesToParse.push_back(queryString);
			}
			regex words_regex("[A-Z]+(\\w*)");
			for (auto value : valuesToParse)
			{
				size_t begin = value.find("(");
				size_t end = value.find(")");
				string tmp = value.substr(begin, end - begin);
				auto words_begin = sregex_iterator(tmp.begin(), tmp.end(), words_regex);
				auto words_end = sregex_iterator();
				while (words_begin != words_end)
				{
					smatch match = *words_begin;
					string match_str = match.str();
					tmp.replace(match.position(), match.length(), this->solver->WILDCARD_STRING);
					words_begin = sregex_iterator(tmp.begin(), tmp.end(), words_regex);
				}
				value = value.replace(begin, end - begin, tmp);
				auto val = this->solver->getGringoModule()->parseValue(value);
				this->headValues.emplace(val, vector<Gringo::Value>());
			}
		}

		vector<Gringo::Value> AspQuery::createQueryValues(string queryString)
		{
			vector<Gringo::Value> ret;
			if (queryString.compare("") == 0)
			{
				return ret;
			}
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
				generateRules(queryString);
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
					generateRules(currentQuery);
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
				generateRules(queryString);
				ret.push_back(this->solver->getGringoModule()->parseValue(queryString));
			}
			return ret;
		}

		bool AspQuery::setQueryString(string queryString)
		{
			if (this->queryString.compare("empty") == 0)
			{
				this->queryString = queryString;
				this->queryValues = this->createQueryValues(queryString);
				for (auto value : this->queryValues)
				{
					this->predicateModelMap.emplace(value, vector<Gringo::Value>());
				}
				return true;
			}
			return false;
		}

		string AspQuery::toString()
		{
			stringstream ss;
			ss << "Query:" << "\n";
			ss << "\tQueryString: " << this->queryString << "\n";
			ss << "\tModels: \n\t\t";
			for (auto model : *this->currentModels)
			{
				for (auto pred : model)
				{
					ss << pred << " ";
				}
			}
			ss << "\n";
			ss << "\tPredicates with models: " << "\n";
			for (auto pair : this->predicateModelMap)
			{
				ss << "\t\tPredicate: " << pair.first << "\n";
				ss << "\t\t\t Model: ";
				for (auto predicate : pair.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
			ss << (this->disjunction ? "\tQuery is disjunction." : "\tQuery is conjunction.") << "\n";
			ss << "\tQuery will be used " << this->lifeTime << " times again.\n";
			ss << "\tRules:" << "\n";
			for (auto rule : this->rules)
			{
				ss << "\t\t" << rule << "\n";
			}
			ss << "\tRules with models:" << "\n";
			for (auto rule : this->ruleModelMap)
			{
				ss << "\t\tRule: " << rule.first << "\n";
				ss << "\t\t\t Model: ";
				for (auto predicate : rule.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
			ss << "\tHeadValues:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\t" << value.first << "\n";
			}
			ss << "\tHeadValues with models:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\tRule: " << value.first << "\n";
				ss << "\t\t\t Model: ";
				for (auto predicate : value.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
			return ss.str();
		}

		string AspQuery::getProgrammSection()
		{
			return this->programmSection;
		}

		void AspQuery::setProgrammSection(string programmSection)
		{
			this->programmSection = programmSection;
		}

		ASPSolver* AspQuery::getSolver()
		{
			return this->solver;
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

		map<Gringo::Value, vector<Gringo::Value>> AspQuery::getPredicateModelMap()
		{
			return this->predicateModelMap;
		}

		bool AspQuery::isDisjunction()
		{
			return this->disjunction;
		}

		vector<Gringo::Value> AspQuery::getQueryValues()
		{
			return this->queryValues;
		}

		map<Gringo::Value, vector<Gringo::Value> > AspQuery::getRuleModelMap()
		{
			return this->ruleModelMap;
		}

		vector<string> AspQuery::getRules()
		{
			return this->rules;
		}

		map<Gringo::Value, vector<Gringo::Value>> AspQuery::getHeadValues()
		{
			return this->headValues;
		}
	} /* namespace reasoner */
} /* namespace alica */

