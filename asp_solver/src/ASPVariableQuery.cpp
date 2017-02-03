/*
 * ASPVariableQuery.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: Stefan Jakob
 */

#include "asp_solver/ASPVariableQuery.h"

#include <regex>
#include <algorithm>
#include "asp_solver/ASPSolver.h"
#include <asp_commons/IASPSolver.h>

namespace reasoner
{

	ASPVariableQuery::ASPVariableQuery(ASPSolver* solver, shared_ptr<reasoner::ASPCommonsTerm> term) :
			ASPQuery(solver, term)
	{
		this->type = ASPQueryType::Variable;
		stringstream ss;
		if (term->getQueryId() == -1)
		{
#ifdef ASPVARIABLEQUERY_DEBUG
			cout << "ASPVariableQuery: Error please set the queryId and add it to any additional Fact or Rule that is going to be queried! " << endl;
#endif
			return;
		}
		ss << "query" << term->getQueryId();
		this->queryProgramSection = ss.str();
#ifdef ASPVARIABLEQUERY_DEBUG
		cout << "ASPVariableQuery: creating query number " << term->getQueryId() << " and program section " << this->queryProgramSection << endl;
#endif
		this->createProgramSection();
		this->createHeadQueryValues(this->term->getRuleHeads());
		auto loaded = this->solver->loadFileFromConfig(this->term->getProgramSection());
		if (loaded)
		{
			this->solver->ground( { {this->term->getProgramSection(), {}}}, nullptr);
		}
		this->solver->ground( { {this->queryProgramSection, {}}}, nullptr);
		this->solver->assignExternal(*(this->external), Gringo::TruthValue::True);
	}

	ASPVariableQuery::~ASPVariableQuery()
	{
	}

	vector<string> ASPVariableQuery::getRules()
	{
		return this->rules;
	}

	void ASPVariableQuery::createProgramSection()
	{
		stringstream ss;
		ss << "external" << this->queryProgramSection;
		this->externalName = ss.str();
		ss.str("");
		ss << "#program " << this->queryProgramSection << "." << endl;
		ss << "#external " << "external" << this->queryProgramSection << "." << endl;
		ss << expandRule(this->term->getQueryRule()) << endl;
		for (auto rule : this->term->getRules())
		{
			ss << expandRule(rule) << endl;
		}
		for (auto fact : this->term->getFacts())
		{
			ss << expandFact(fact) << endl;
		}
#ifdef ASPVARIABLEQUERY_DEBUG
		cout << "ASPVariableQuery: create program section: \n" << ss.str();
#endif
		this->solver->add(this->queryProgramSection, {}, ss.str());
		this->external = make_shared<Gringo::Value>(this->solver->parseValue(this->externalName));
	}

	void ASPVariableQuery::removeExternal()
	{
		this->solver->releaseExternal(*(this->external));
	}

	string ASPVariableQuery::expandRule(string rule)
	{
		supplementary::Configuration::trim(rule);
		rule = rule.substr(0, rule.size() - 1);
		stringstream ss;
		ss << rule << ", " << this->externalName << ".";
#ifdef ASPVARIABLEQUERY_DEBUG
		cout << "ASPVariableQuery: rule: " << ss.str() << endl;
#endif
		return ss.str();

	}

	string ASPVariableQuery::expandFact(string fact)
	{
		supplementary::Configuration::trim(fact);
		fact = fact.substr(0, fact.size() - 1);
		stringstream ss;
		ss << fact << " :- " << this->externalName << ".";
#ifdef ASPVARIABLEQUERY_DEBUG
		cout << "ASPVariableQuery: fact: " << ss.str() << endl;
#endif
		return ss.str();
	}

	void ASPVariableQuery::createHeadQueryValues(vector<string> queryVec)
	{
		vector<string> valuesToParse;
		for (auto queryString : queryVec)
		{
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
			auto val = this->solver->parseValue(value);
			this->headValues.emplace(val, vector<Gringo::Value>());
		}
	}

	ASPQueryType ASPVariableQuery::getType()
	{
		return this->type;
	}

} /* namespace reasoner */

