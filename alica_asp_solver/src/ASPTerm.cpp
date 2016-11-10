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
			this->type = ASPQueryType::Undefined;
		}

		ASPTerm::~ASPTerm()
		{
		}

		bool ASPTerm::setRule(string rule)
		{
			// not allowed to override an existing rule
			if (!this->rule.empty())
			{
				return false;
			}

			this->rule = supplementary::Configuration::trim(rule);
			size_t endOfHead = rule.find(":-");
			if (endOfHead != string::npos)
			{
				// for rules (including variables)
				size_t startOfBody = endOfHead + 2;
				this->head = supplementary::Configuration::trim(rule.substr(0, endOfHead));
				this->body = supplementary::Configuration::trim(rule.substr(startOfBody, rule.size() - startOfBody - 1));
			}
			else
			{
				// for ground literals
				this->head = this->rule;
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

		long ASPTerm::getId()
		{
			return id;
		}

		void ASPTerm::setId(long id)
		{
			this->id = id;
		}

	} /* namespace reasoner */
} /* namespace alica */

