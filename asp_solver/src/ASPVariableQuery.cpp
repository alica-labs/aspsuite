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

	string ASPVariableQuery::expandQueryRuleModuleProperty(string rule)
	{
		//TODO discuss how to handle rules
		if (rule.find(":-") == 0)
		{
			return rule;
		}
		stringstream ss;
		ss << this->expandRuleModuleProperty(rule);
		if (this->getTerm()->getRules().size() > 0)
		{
			ss << endl;
			rule = supplementary::Configuration::trim(rule);
			size_t endOfQueryRuleHead = rule.find(":-");
			string queryRuleHead = (supplementary::Configuration::trim(rule.substr(0, endOfQueryRuleHead)));
			queryRuleHead = replaceHeadPredicates(queryRuleHead);
			string queryRuleBody = (supplementary::Configuration::trim(
					rule.substr(endOfQueryRuleHead + 2, rule.size() - endOfQueryRuleHead + 1)));
			for (auto r : this->term->getRules())
			{
				size_t endOfRuleHead = r.find(":-");
				string head;

				// for rules (including variables)
				size_t startOfBody = endOfRuleHead + 2;
				head = (supplementary::Configuration::trim(r.substr(0, endOfRuleHead)));
				auto headPredicates = createHeadPredicates(head);
#ifdef ASPVARIABLEQUERY_DEBUG
				cout << "ASPVariableQuery: Head: " << head << endl;
#endif
				for (auto pred : headPredicates)
				{
					replaceFittingPredicate(queryRuleBody, pred);
#ifdef ASPVARIABLEQUERY_DEBUG
					cout << "ASPVariableQuery: Replaced body: " << queryRuleBody << endl;
#endif
				}
				for (auto fact : this->term->getFacts())
				{
					replaceFittingPredicate(queryRuleBody, fact);
#ifdef ASPVARIABLEQUERY_DEBUG
					cout << "ASPVariableQuery: Replaced body: " << body << endl;
#endif
				}
			}
#ifdef ASPVARIABLEQUERY_DEBUG
			cout << "ASPVariableQuery: queryRule: " << ss.str() << endl;
#endif
			queryRuleBody = queryRuleBody.substr(0, queryRuleBody.size() - 1);
			ss << queryRuleHead << " :- " << queryRuleBody << ", " << this->externalName << ".";
		}
		return ss.str();
	}

	string ASPVariableQuery::expandFactModuleProperty(string fact)
	{
		fact = supplementary::Configuration::trim(fact);
		fact = fact.substr(0, fact.size() - 1);
		stringstream ss;
		ss << this->queryProgramSection << "(" << fact << ") :- " << this->externalName << ".";
#ifdef ASPVARIABLEQUERY_DEBUG
		cout << "ASPVariableQuery: fact: " << ss.str() << endl;
#endif
		return ss.str();
	}

	void ASPVariableQuery::replaceFittingPredicate(string& ruleBody, string predicate)
	{
		auto tmp = predicate.substr(0, predicate.find("("));
		auto pos = ruleBody.find(tmp);
		if (pos != string::npos)
		{
			stringstream ss;
			ss << this->queryProgramSection << "(" << tmp << "("
					<< ruleBody.substr(pos + tmp.size() + 1, ruleBody.find(")", pos) - pos - tmp.size() - 1) << "))";
			ruleBody.replace(pos, ruleBody.find(")", pos) - pos + 1, ss.str());
		}
	}

	string ASPVariableQuery::expandRuleModuleProperty(string rule)
	{
		rule = supplementary::Configuration::trim(rule);
		if (rule.find(":-") == 0)
		{
			return rule;
		}
		else
		{
			size_t endOfHead = rule.find(":-");
			string head;
			string body;

			// for rules (including variables)
			size_t startOfBody = endOfHead + 2;
			head = (supplementary::Configuration::trim(rule.substr(0, endOfHead)));
			body = (supplementary::Configuration::trim(rule.substr(startOfBody, rule.size() - startOfBody - 1)));
#ifdef ASPVARIABLEQUERY_DEBUG
			cout << "ASPVariableQuery: Head: " << head << endl;
			cout << "ASPVariableQuery: Body: " << body << endl;
#endif
			for (auto fact : this->term->getFacts())
			{
				replaceFittingPredicate(body, fact);
#ifdef ASPVARIABLEQUERY_DEBUG
				cout << "ASPVariableQuery: Replaced body: " << body << endl;
#endif
			}
			head = replaceHeadPredicates(head);
#ifdef ASPVARIABLEQUERY_DEBUG
			cout << "ASPVariableQuery: Replaced head: " << head << endl;
#endif
			rule = rule.substr(0, rule.size() - 1);
			stringstream ss;
			ss << head << " :- " << body << ", " << this->externalName << ".";
#ifdef ASPVARIABLEQUERY_DEBUG
			cout << "ASPVariableQuery: rule: " << ss.str() << endl;
#endif
			return ss.str();
		}
	}

	string ASPVariableQuery::replaceHeadPredicates(string head)
	{
		stringstream ss;
		if (head.find(",") != string::npos && head.find(";") == string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string currentQuery = "";
			while (start != string::npos)
			{
				end = head.find(")", start);
				if (end == string::npos)
				{
					break;
				}
				currentQuery = head.substr(start, end - start + 1);
				currentQuery = supplementary::Configuration::trim(currentQuery);
				ss << this->queryProgramSection << "(" << currentQuery << ")";
				start = head.find(",", end);
				if (start != string::npos)
				{
					ss << ",";
					start += 1;
				}
			}
		}
		else if (head.find(";") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string currentQuery = "";
			while (start != string::npos)
			{
				end = head.find(")", start);
				if (end == string::npos)
				{
					break;
				}
				currentQuery = head.substr(start, end - start + 1);
				currentQuery = supplementary::Configuration::trim(currentQuery);
				ss << this->queryProgramSection << "(" << currentQuery << ")";
				start = head.find(";", end);
				if (start != string::npos)
				{
					ss << ";";
					start += 1;
				}
			}
		}
		else
		{
			ss << this->queryProgramSection << "(" << head << ")";
		}
		return ss.str();
	}

	vector<string> ASPVariableQuery::createHeadPredicates(string head)
	{
		vector<string> ret;
		if (head.find(",") != string::npos && head.find(";") == string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string currentQuery = "";
			while (start != string::npos)
			{
				end = head.find(")", start);
				if (end == string::npos)
				{
					break;
				}
				currentQuery = head.substr(start, end - start + 1);
				currentQuery = supplementary::Configuration::trim(currentQuery);
				ret.push_back(currentQuery);
				start = head.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
			}
		}
		else if (head.find(";") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string currentQuery = "";
			while (start != string::npos)
			{
				end = head.find(")", start);
				if (end == string::npos)
				{
					break;
				}
				currentQuery = head.substr(start, end - start + 1);
				currentQuery = supplementary::Configuration::trim(currentQuery);
				ret.push_back(currentQuery);
				start = head.find(";", end);
				if (start != string::npos)
				{
					start += 1;
				}
			}
		}
		else
		{
			ret.push_back(head);
		}
		return ret;
	}

	void ASPVariableQuery::createProgramSection()
	{
		stringstream ss;
		ss << "external" << this->queryProgramSection;
		this->externalName = ss.str();
		ss.str("");
		ss << "#program " << this->queryProgramSection << "." << endl;
		ss << "#external " << "external" << this->queryProgramSection << "." << endl;
		ss << expandQueryRuleModuleProperty(this->term->getQueryRule()) << endl;
//#ifdef ASPVARIABLEQUERY_DEBUG
		cout << expandQueryRuleModuleProperty(this->term->getQueryRule()) << endl;
//#endif
		for (auto rule : this->term->getRules())
		{
			ss << expandRuleModuleProperty(rule) << endl;
		}
		for (auto fact : this->term->getFacts())
		{
			ss << expandFactModuleProperty(fact) << endl;
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

//	string ASPVariableQuery::expandRule(string rule)
//	{
//		rule = supplementary::Configuration::trim(rule);
//		rule = rule.substr(0, rule.size() - 1);
//		stringstream ss;
//		ss << rule << ", " << this->externalName << ".";
//#ifdef ASPVARIABLEQUERY_DEBUG
//		cout << "ASPVariableQuery: rule: " << ss.str() << endl;
//#endif
//		return ss.str();
//
//	}
//
//	string ASPVariableQuery::expandFact(string fact)
//	{
//		fact = supplementary::Configuration::trim(fact);
//		fact = fact.substr(0, fact.size() - 1);
//		stringstream ss;
//		ss << fact << " :- " << this->externalName << ".";
//#ifdef ASPVARIABLEQUERY_DEBUG
//		cout << "ASPVariableQuery: fact: " << ss.str() << endl;
//#endif
//		return ss.str();
//	}

	void ASPVariableQuery::createHeadQueryValues(vector<string> queryVec)
	{
		vector<string> valuesToParse;
		for (auto queryString : queryVec)
		{
			auto vec = createHeadPredicates(queryString);
			for (auto s : vec)
			{
				valuesToParse.push_back(s);
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
			stringstream ss;
			ss << this->queryProgramSection << "(" << value << ")";
			auto val = this->solver->parseValue(ss.str());
			this->headValues.emplace(val, vector<Gringo::Value>());
		}
	}

	ASPQueryType ASPVariableQuery::getType()
	{
		return this->type;
	}

} /* namespace reasoner */

