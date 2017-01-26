/*
 * Term.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include "../include/asp_commons/ASPTerm.h"

#include <SystemConfig.h>

namespace reasoner
{

	ASPTerm::ASPTerm(int lifeTime)
	{
		this->programSection = "";
		this->id = -1;
		this->lifeTime = lifeTime;
		this->externals = nullptr;
		this->numberOfModels = "";
		this->type = ASPQueryType::Undefined;
		this->queryId = -1;
	}

	ASPTerm::~ASPTerm()
	{
	}

	void ASPTerm::addRule(string rule)
	{
		if (rule.empty())
		{
			return;
		}

		rule = supplementary::Configuration::trim(rule);
		size_t endOfHead = rule.find(":-");
		if (endOfHead != string::npos)
		{
			// for rules (including variables)
			size_t startOfBody = endOfHead + 2;
			this->heads.push_back(supplementary::Configuration::trim(rule.substr(0, endOfHead)));
			this->bodies.push_back(
					supplementary::Configuration::trim(rule.substr(startOfBody, rule.size() - startOfBody - 1)));
		}
		else
		{
			// for ground literals
			this->heads.push_back(rule);
		}
		this->rules.push_back(rule);
//			return true;
	}

	void ASPTerm::addFact(string fact)
	{
		if (fact.find(".") == string::npos)
		{
			this->facts.push_back(fact + ".");
		}
		else
		{
			this->facts.push_back(fact);
		}
	}

	vector<string> ASPTerm::getRuleHeads()
	{
		return this->heads;
	}

	vector<string> ASPTerm::getRuleBodies()
	{
		return this->bodies;
	}

	string ASPTerm::getProgramSection()
	{
		return this->programSection;
	}

	void ASPTerm::setProgramSection(string programSection)
	{
		this->programSection = programSection;
	}

	int ASPTerm::getLifeTime()
	{
		return this->lifeTime;
	}

	vector<string> ASPTerm::getRules()
	{
		return this->rules;
	}

	vector<string> ASPTerm::getFacts()
	{
		return this->facts;
	}

	void ASPTerm::setExternals(shared_ptr<map<string, bool>> externals)
	{
		this->externals = externals;
	}

	shared_ptr<map<string, bool> > ASPTerm::getExternals()
	{
		return this->externals;
	}

	string ASPTerm::getNumberOfModels()
	{
		return this->numberOfModels;
	}

	void ASPTerm::setNumberOfModels(string numberOfModels)
	{
		this->numberOfModels = numberOfModels;
	}

	ASPQueryType ASPTerm::getType()
	{
		return this->type;
	}

	void ASPTerm::setType(ASPQueryType type)
	{
		this->type = type;
	}

	long ASPTerm::getId()
	{
		return id;
	}

	void ASPTerm::setId(long id)
	{
		this->id = id;
	}
	/**
	 * The query id has to be added to any predicate which is added to the program, naming rule
	 * heads and facts!
	 * An unique id is given by the ASPSolver!
	 */
	int ASPTerm::getQueryId()
	{
		return queryId;
	}

	/**
	 * The query id has to be added to any predicate which is added to the program, naming rule
	 * heads and facts!
	 * An unique id is given by the ASPSolver!
	 */
	void ASPTerm::setQueryId(int queryId)
	{
		this->queryId = queryId;
	}

} /* namespace reasoner */

