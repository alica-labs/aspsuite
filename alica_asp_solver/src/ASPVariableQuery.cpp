/*
 * ASPVariableQuery.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#include <alica_asp_solver/ASPVariableQuery.h>
#include <alica_asp_solver/ASPSolver.h>
#include <regex>
#include <algorithm>

namespace alica
{
	namespace reasoner
	{

		ASPVariableQuery::ASPVariableQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term) :
				ASPQuery(solver, term)
		{
			this->type = ASPQueryType::Variable;
			this->queryId = this->solver->getQueryCounter();
			stringstream ss;
			ss << "queryExternal" << this->queryId;
			this->queryProgramSection = ss.str();
			cout << "ASPVariableQuery: creating query with number" << this->queryId << " and program section " << this->queryProgramSection << endl;
			this->createExternal();
			auto loaded = this->solver->loadFileFromConfig(this->term->getProgrammSection());
			this->createHeadQueryValues(this->term->getRuleHead());
#ifdef ASPSolver_DEBUG
			cout << "ASPSolver: Query contains rule: " << this->term->getRule() << endl;
#endif
			this->addRule(this->queryProgramSection, this->term->getRule(), false);
			for (auto fact : this->term->getFacts())
			{
#ifdef ASPSolver_DEBUG
				cout << "ASPSolver: Query contains fact: " << fact << endl;
#endif
				this->addFact(this->queryProgramSection, fact, false);
			}
			if (loaded)
			{
				this->solver->getClingo()->ground( { {this->term->getProgrammSection(), {}}}, nullptr);
			}
			this->solver->getClingo()->ground( { {this->queryProgramSection, {}}}, nullptr);
			this->solver->getClingo()->assignExternal(*(this->external), Gringo::TruthValue::True);
		}

		ASPVariableQuery::~ASPVariableQuery()
		{
		}

		vector<string> ASPVariableQuery::getRules()
		{
			return this->rules;
		}

		void ASPVariableQuery::addRule(string programSection, string rule, bool ground)
		{
			rule = expandRule(rule);
			this->solver->getClingo()->add(programSection, {}, rule);
			this->rules.push_back(rule);
			if (ground)
			{
				this->solver->ground( { {programSection, {}}}, nullptr);
			}
		}

		void ASPVariableQuery::addFact(string programSection, string fact, bool ground)
		{
			fact = expandFact(fact);
			this->solver->getClingo()->add(programSection, {}, fact);
			this->rules.push_back(fact);
			if (ground)
			{
				this->solver->ground( { {programSection, {}}}, nullptr);
			}
		}

		void ASPVariableQuery::createExternal()
		{
			stringstream ss;
			ss << "external" << this->queryProgramSection;
			this->externalName = ss.str();
			ss.str("");
			ss << "#external " << this->externalName << ".";
			this->solver->getClingo()->add(this->queryProgramSection, {}, ss.str());
			this->external = make_shared<Gringo::Value>(this->solver->getGringoModule()->parseValue(this->externalName));
//			this->solver->getClingo()->ground({ {this->queryProgramSection, {}}}, nullptr);
		}

		void ASPVariableQuery::removeExternal()
		{
			//TODO change clingo version to get release external
			this->solver->getClingo()->assignExternal(*(this->external),  Gringo::TruthValue::False);
		}

		string ASPVariableQuery::expandRule(string rule)
		{
			stringstream ss;
			ss << ", " << this->externalName;
			string toAdd = ss.str();
			ss.str("");
			rule = rule.substr(0, rule.size()-1);
			ss << rule << toAdd << ".";
			cout << "ASPVariableQuery: rule: " << ss.str() << endl;
			return ss.str();

		}

		string ASPVariableQuery::expandFact(string fact)
		{
			stringstream ss;
			ss << " :- " << this->externalName;
			string toAdd = ss.str();
			ss.str("");
			fact = fact.substr(0, fact.size()-1);
			ss << fact << toAdd << ".";
			cout << "ASPVariableQuery: fact: " << ss.str() << endl;
			return ss.str();
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

		ASPQueryType ASPVariableQuery::getType()
		{
			return this->type;
		}

	} /* namespace reasoner */
} /* namespace alica */

