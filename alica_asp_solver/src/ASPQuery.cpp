/*
 * AspQuery.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/ASPQuery.h>
#include <alica_asp_solver/ASPSolver.h>
#include <clingo/clingocontrol.hh>
#include <Logging.h>

namespace alica
{
	namespace reasoner
	{

		ASPQuery::ASPQuery(ASPSolver* solver, shared_ptr<alica::reasoner::ASPTerm> term)
		{
			this->type = ASPQueryType::Undefined;
			this->solver = solver;
			this->term = term;
			this->programSection = term->getProgramSection();
			this->lifeTime = term->getLifeTime();
			this->currentModels = make_shared<vector<Gringo::ValVec>>();
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

		void ASPQuery::saveHeadValuePair(Gringo::Value key, Gringo::Value value)
		{
			auto entry = this->headValues.find(key);
			if (entry != this->headValues.end())
			{
				entry->second.push_back(value);
			}
		}

		void ASPQuery::onModel(ClingoModel& clingoModel)
		{
			this->getCurrentModels()->push_back(clingoModel.atoms(Gringo::Model::SHOWN));
			//	cout << "ASPQuery: processing query '" << queryMapPair.first << "'" << endl;

			// determine the domain of the query predicate
			for (auto value : this->getHeadValues())
			{
				cout << "ASPSolver::onModel: " << value.first << endl;
				auto it = clingoModel.out.domains.find(value.first.sig());
				if (it == clingoModel.out.domains.end())
				{
					//cout << "ASPQuery: Didn't find any suitable domain!" << endl;
					continue;
				}

				for (auto& domainPair : it->second.domain)
				{
					//cout << "ASPQuery: Inside domain-loop!" << endl;

					if (&(domainPair.second)
							&& clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
					{
						//cout << "ASPQuery: Found true literal '" << domainPair.first << "'" << endl;

						if (this->checkMatchValues(&value.first, &domainPair.first))
						{
							//cout << "ASPQuery: Literal '" << domainPair.first << "' matched!" << endl;
							this->saveHeadValuePair(value.first, domainPair.first);
						}
					}
				}
			}
		}

		bool ASPQuery::checkMatchValues(const Gringo::Value* value1, const Gringo::Value* value2)
		{
			if (value2->type() != Gringo::Value::Type::FUNC)
				return false;

			if (value1->name() != value2->name())
				return false;

			if (value1->args().size() != value2->args().size())
				return false;

			for (uint i = 0; i < value1->args().size(); ++i)
			{
				Gringo::Value arg = value1->args()[i];

				if (arg.type() == Gringo::Value::Type::ID && arg.name() == ASPSolver::WILDCARD_STRING)
					continue;

				if (arg.type() == Gringo::Value::Type::FUNC && value2->args()[i].type() == Gringo::Value::Type::FUNC)
				{
					if (false == checkMatchValues(&arg, &value2->args()[i]))
						return false;
				}
				else if (arg != value2->args()[i])
					return false;
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

		ASPSolver* ASPQuery::getSolver()
		{
			return this->solver;
		}

		shared_ptr<vector<Gringo::ValVec>> ASPQuery::getCurrentModels()
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

		map<Gringo::Value, vector<Gringo::Value>> ASPQuery::getHeadValues()
		{
			return this->headValues;
		}

		shared_ptr<alica::reasoner::ASPTerm> ASPQuery::getTerm()
		{
			return term;
		}

		ASPQueryType ASPQuery::getType()
		{
			return type;
		}
	} /* namespace reasoner */
} /* namespace alica */

