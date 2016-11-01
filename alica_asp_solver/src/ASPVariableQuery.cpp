/*
 * ASPVariableQuery.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#include <alica_asp_solver/ASPVariableQuery.h>
#include <alica_asp_solver/ASPSolver.h>
#include <regex>

namespace alica
{
	namespace reasoner
	{

		ASPVariableQuery::ASPVariableQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term) :
				ASPQuery(solver, term)
		{
			auto loaded = this->solver->loadFileFromConfig(this->term->getProgrammSection());
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

		ASPVariableQuery::~ASPVariableQuery()
		{
		}

		vector<string> ASPVariableQuery::getRules()
		{
			return this->rules;
		}

		void ASPVariableQuery::addRule(string domainName, string rule, bool ground)
		{
			this->solver->getClingo()->add(domainName, {}, rule);
			this->rules.push_back(rule);
			if (ground)
			{
				this->solver->ground( { {domainName, {}}}, nullptr);
			}
		}

		void ASPVariableQuery::createHeadQueryValues(string queryString)
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

	} /* namespace reasoner */
} /* namespace alica */
