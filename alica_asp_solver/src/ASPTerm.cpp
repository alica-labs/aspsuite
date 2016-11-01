/*
 * Term.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/ASPTerm.h>
#include <SystemConfig.h>

namespace alica
{
	namespace reasoner
	{

		ASPTerm::ASPTerm(int lifeTime)
		{
			this->programmSection = "";
			this->rule = "";
			this->head = "";
			this->body = "";
			this->lifeTime = lifeTime;
			this->externals = nullptr;
			this->numberOfModels = string::npos;
		}

		ASPTerm::~ASPTerm()
		{
		}

		bool ASPTerm::setRule(string rule)
		{
			if (this->rule.compare("") != 0)
			{
				return false;
			}
			this->rule = rule;
			if (rule.find(":-") != string::npos)
			{
				size_t endOfHead = rule.find(":-");
				size_t startOfBody = endOfHead + 2;
				this->head = rule.substr(0, endOfHead);
				this->body = rule.substr(startOfBody, rule.size() - startOfBody - 1);
				this->head = supplementary::Configuration::trim(this->head);
				this->body = supplementary::Configuration::trim(this->body);
			}
			return true;
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

		string ASPTerm::getRuleHead()
		{
			return this->head;
		}

		string ASPTerm::getRuleBody()
		{
			return this->body;
		}

		string ASPTerm::getProgrammSection()
		{
			return this->programmSection;
		}

		void ASPTerm::setProgrammSection(string programmSection)
		{
			this->programmSection = programmSection;
		}

		int ASPTerm::getLifeTime()
		{
			return this->lifeTime;
		}

		string ASPTerm::getRule()
		{
			return this->rule;
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

	} /* namespace reasoner */
} /* namespace alica */


