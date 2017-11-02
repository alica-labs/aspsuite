/*
 * AspQuery.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#include <ASPCommonsTerm.h>
#include "asp_commons/ASPQuery.h"

#include "asp_commons/AnnotatedValVec.h"
#include "asp_commons/IASPSolver.h"

namespace reasoner
{

	ASPQuery::ASPQuery(IASPSolver* solver, shared_ptr<reasoner::ASPCommonsTerm> term)
	{
		this->type = ASPQueryType::Undefined;
		this->solver = solver;
		this->term = term;
		this->programSection = term->getProgramSection();
		this->lifeTime = term->getLifeTime();
		this->currentModels = make_shared<vector<Gringo::SymVec>>();
	}

	ASPQuery::~ASPQuery()
	{
	}

	void ASPQuery::reduceLifeTime()
	{
		if (this->lifeTime > 0)
		{
			this->lifeTime--;
		}
	}

	void ASPQuery::saveHeadValuePair(Gringo::Symbol key, Gringo::Symbol value)
	{
		auto entry = this->headValues.find(key);
		if (entry != this->headValues.end())
		{
			//TODO remove
			if (find(entry->second.begin(), entry->second.end(), value) == entry->second.end())
			{
				entry->second.push_back(value);
			}
		}
	}

	bool ASPQuery::checkMatchValues(Gringo::Symbol value1, Gringo::Symbol value2)
	{
		if (value2.type() != Gringo::SymbolType::Fun)
		{
			return false;
		}

		if (value1.name() != value2.name())
		{
			return false;
		}
		if (value1.args().size != value2.args().size)
		{
			return false;
		}

		for (uint i = 0; i < value1.args().size; ++i)
		{
			Gringo::Symbol arg = value1.args()[i];
			if (arg.type() == Gringo::SymbolType::Fun && std::string(arg.name().c_str()) == IASPSolver::WILDCARD_STRING)
			{
				continue;
			}

			if (arg.type() == Gringo::SymbolType::Fun && value2.args()[i].type() == Gringo::SymbolType::Fun)
			{
				if (false == checkMatchValues(arg, value2.args()[i]))
				{
					return false;
				}
			}
			else if (arg != value2.args()[i])
			{
				return false;
			}
		}
		return true;
	}
	string ASPQuery::toString()
	{
		stringstream ss;
		ss << "Query:" << "\n";
		ss << "\tModels: \n\t\t";
		int counter = 0;
		for (auto model : *this->currentModels)
		{
			counter++;
			ss << "Number " << counter << ":\n\t\t\t";
			for (auto pred : model)
			{
				ss << pred << " ";
			}
			ss << "\n";
		}
		ss << "\tQuery will be used " << this->lifeTime << " times again.\n";

		if (this->getType() == ASPQueryType::Facts)
		{
			ss << "\tQuery is of type Facts.\n";
			ss << "\tFacts:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\t" << value.first << "\n";
			}
			ss << "\tFacts in models:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\tFact: " << value.first << "\n";
				ss << "\t\t\t In Model: ";
				for (auto predicate : value.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
		}
		else
		{
			ss << "\tQuery is of type Variable.\n";
			ss << "\tRuleHeadValues:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\t" << value.first << "\n";
			}
			ss << "\tRuleHeadValues with models:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\tRuleHead: " << value.first << "\n";
				ss << "\t\t\t Grounded in Model: ";
				for (auto predicate : value.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
		}
		return ss.str();
	}

	string ASPQuery::getProgrammSection()
	{
		return this->programSection;
	}

	void ASPQuery::setProgrammSection(string programSection)
	{
		this->programSection = programSection;
	}

	IASPSolver* ASPQuery::getSolver()
	{
		return this->solver;
	}

	shared_ptr<vector<Gringo::SymVec>> ASPQuery::getCurrentModels()
	{
		return this->currentModels;
	}

	int ASPQuery::getLifeTime()
	{
		return this->lifeTime;
	}

	void ASPQuery::setLifeTime(int lifeTime)
	{
		this->lifeTime = lifeTime;
	}

	vector<string> ASPQuery::getRules()
	{
		return this->rules;
	}

	map<Gringo::Symbol, Gringo::SymVec>& ASPQuery::getHeadValues()
	{
		return this->headValues;
	}

	shared_ptr<reasoner::ASPCommonsTerm> ASPQuery::getTerm()
	{
		return term;
	}

	ASPQueryType ASPQuery::getType()
	{
		return type;
	}

} /* namespace reasoner */
