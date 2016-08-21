/*
 * Term.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/Term.h>
#include <SystemConfig.h>

namespace alica
{
	namespace reasoner
	{

		Term::Term(int lifeTime)
		{
			this->backgroundFile = "";
			this->rule = "";
			this->head = "";
			this->body = "";
			this->lifeTime = lifeTime;
		}

		Term::~Term()
		{
		}

		bool Term::setRule(std::string rule)
		{
			if (this->rule.compare("") != 0)
			{
				return false;
			}
			this->rule = rule;
			if (rule.find(":-") != std::string::npos)
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

		void Term::addFact(std::string fact)
		{
			if(fact.find(".") == std::string::npos)
			{
				this->facts.push_back(fact + ".");
			}
			else
			{
				this->facts.push_back(fact);
			}
		}


		std::string Term::getRuleHead()
		{
			return this->head;
		}

		std::string Term::getRuleBody()
		{
			return this->body;
		}

		std::string Term::getBackGroundFileName()
		{
			return this->backgroundFile;
		}

		void Term::setBackgroundFileName(std::string backgroundFile)
		{
			this->backgroundFile = backgroundFile;
		}

		int Term::getLifeTime()
		{
			return lifeTime;
		}

		std::string Term::getRule()
		{
			return this->rule;
		}

		 std::vector<std::string> Term::getFacts()
		{
			return facts;
		}
	} /* namespace reasoner */
} /* namespace alica */


