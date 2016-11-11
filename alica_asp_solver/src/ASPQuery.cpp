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
			this->solver = solver;
			this->term = term;
			this->programmSection = term->getProgrammSection();
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

//		void ASPQuery::generateRules(string queryString)
//		{
//			stringstream ss;
//			ss << "queryHolds(query" << this->solver->getQueryCounter() << ")";
//			string head = ss.str();
//			ss << " :- " << queryString << ".";
//			string rule = ss.str();
//			// FIXME: This is a problem if somebody already used X in the queryString.
//			if (queryString.find(ASPSolver::WILDCARD_STRING) != string::npos)
//			{
//				int pos = rule.find(ASPSolver::WILDCARD_STRING);
//				rule.replace(pos, ASPSolver::WILDCARD_STRING.length(), "X");
//			}
//
//			this->solver->getClingo()->add(this->programmSection, {}, rule);
//			this->ruleModelMap.emplace(this->solver->getGringoModule()->parseValue(head), vector<Gringo::Value>());
//			this->rules.push_back(rule);
//
//			LOG(rule);
//		}

		string ASPQuery::toString()
		{
			stringstream ss;
			ss << "Query:" << "\n";
			ss << "\tModels: \n\t\t";
			for (auto model : *this->currentModels)
			{
				for (auto pred : model)
				{
					ss << pred << " ";
				}
			}
			ss << "\n";
			ss << "\tQuery will be used " << this->lifeTime << " times again.\n";
//			ss << "\tRules:" << "\n";
//			for (auto rule : this->rules)
//			{
//				ss << "\t\t" << rule << "\n";
//			}
//			ss << "\tRules with models:" << "\n";
//			for (auto rule : this->ruleModelMap)
//			{
//				ss << "\t\tRule: " << rule.first << "\n";
//				ss << "\t\t\t Model: ";
//				for (auto predicate : rule.second)
//				{
//					ss << predicate << " ";
//				}
//				ss << "\n";
//			}
			ss << "\tHeadValues:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\t" << value.first << "\n";
			}
			ss << "\tHeadValues with models:" << "\n";
			for (auto value : this->headValues)
			{
				ss << "\t\tRule: " << value.first << "\n";
				ss << "\t\t\t Model: ";
				for (auto predicate : value.second)
				{
					ss << predicate << " ";
				}
				ss << "\n";
			}
			return ss.str();
		}

		string ASPQuery::getProgrammSection()
		{
			return this->programmSection;
		}

		void ASPQuery::setProgrammSection(string programmSection)
		{
			this->programmSection = programmSection;
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
	} /* namespace reasoner */
} /* namespace alica */


