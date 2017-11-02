/*
 * ASPFactsQuery.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: stefan
 */

#include "asp_solver/ASPFactsQuery.h"

#include "asp_solver/ASPSolver.h"

#include <asp_commons/IASPSolver.h>

namespace reasoner
{

	ASPFactsQuery::ASPFactsQuery(ASPSolver* solver, shared_ptr<ASPCommonsTerm> term) :
			ASPQuery(solver, term)
	{
		this->type = ASPQueryType::Facts;
		this->createQueryValues(term->getRuleHeads());
		this->currentModels = make_shared<vector<Gringo::SymVec>>();

		if (!term->getProgramSection().empty())
		{
			auto loaded = this->solver->loadFileFromConfig(term->getProgramSection());
#ifdef ASPSolver_DEBUG
			cout << "ASPSolver: Query contains rule: " << this->term->getRule() << endl;
#endif
			for (auto fact : this->term->getFacts())
			{
#ifdef ASPSolver_DEBUG
				cout << "ASPSolver: Query contains fact: " << fact << endl;
#endif
			}
			if (loaded)
			{
				this->solver->ground( { {Gringo::String(term->getProgramSection().c_str()), {}}}, nullptr);
			}
		}
	}

	ASPFactsQuery::~ASPFactsQuery()
	{
	}

	void ASPFactsQuery::createQueryValues(vector<string> queryVec)
	{
		for (auto queryString : queryVec)
		{
			if (queryString.compare("") == 0)
			{
				return;
			}
			if (queryString.find(",") != string::npos)
			{
				size_t start = 0;
				size_t end = string::npos;
				string currentQuery = "";
				while (start != string::npos)
				{
					end = queryString.find(")", start);
					if (end == string::npos || end == queryString.size())
					{
						break;
					}
					currentQuery = queryString.substr(start, end - start + 1);
					currentQuery = supplementary::Configuration::trim(currentQuery);
					this->headValues.emplace(this->solver->parseValue(currentQuery), vector<Gringo::Symbol>());
					start = queryString.find(",", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else
			{
				this->headValues.emplace(this->solver->parseValue(queryString), vector<Gringo::Symbol>());
			}
		}
	}

	map<Gringo::Symbol, vector<Gringo::Symbol> > ASPFactsQuery::getFactModelMap()
	{
		return this->factModelMap;
	}

	void ASPFactsQuery::setFactModelMap(map<Gringo::Symbol, vector<Gringo::Symbol> > factModelMap)
	{
		this->factModelMap = factModelMap;
	}

	void ASPFactsQuery::saveSatisfiedFact(Gringo::Symbol key, Gringo::Symbol value)
	{
		auto entry = this->factModelMap.find(key);
		if (entry != this->factModelMap.end())
		{
			entry->second.push_back(value);
		}
	}

	shared_ptr<map<Gringo::Symbol, vector<Gringo::Symbol> > > ASPFactsQuery::getSatisfiedFacts()
	{
		shared_ptr<map<Gringo::Symbol, vector<Gringo::Symbol> > > ret = make_shared<
				map<Gringo::Symbol, vector<Gringo::Symbol> > >();
		for (auto pair : this->factModelMap)
		{
			if (pair.second.size() > 0)
			{
				ret->emplace(pair);
			}
		}
		return ret;
	}

	bool ASPFactsQuery::factsExistForAtLeastOneModel()
	{
		for (auto queryValue : this->headValues)
		{
			if (queryValue.second.size() > 0)
			{
				return true;
			}
		}
		return false;
	}

	bool ASPFactsQuery::factsExistForAllModels()
	{
		for (auto queryValue : this->headValues)
		{
			if (queryValue.second.size() == 0)
			{
				return false;
			}
		}
		return true;
	}

	void ASPFactsQuery::removeExternal()
	{
	}

	vector<pair<Gringo::Symbol, ASPTruthValue> > ASPFactsQuery::getASPTruthValues()
	{
		vector<pair<Gringo::Symbol, ASPTruthValue>> ret;
		for (auto iter : this->getHeadValues())
		{
			if (iter.second.size() == 0)
			{
				ret.push_back(pair<Gringo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::Unknown));
			}
			else
			{
				if (iter.second.at(0).sign())
				{
					ret.push_back(pair<Gringo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::True));
				}
				else
				{
					ret.push_back(pair<Gringo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::False));
				}
			}

		}
		return ret;
	}

	void ASPFactsQuery::onModel(ClingoModel& clingoModel)
	{
		Gringo::SymVec vec;
		auto tmp = clingoModel.atoms(clingo_show_type_shown);
		for (int i = 0; i < tmp.size; i++)
		{
			vec.push_back(tmp[i]);
		}
		this->getCurrentModels()->push_back(vec);
		//	cout << "ASPQuery: processing query '" << queryMapPair.first << "'" << endl;

		// determine the domain of the query predicate
//		for(auto it = ((ASPSolver*)this->solver)->clingo->out_->predDoms().begin(); it != ((ASPSolver*)this->solver)->clingo->out_->predDoms().end(); it++)
//		{
//			cout << "ASPFactsQuery: " << *(*it) << endl;
//		}
		for (auto value : this->getHeadValues())
		{
#ifdef ASPQUERY_DEBUG
			cout << "ASPFactsQuery::onModel: " << value.first << endl;
#endif
			auto it = ((ASPSolver*)this->solver)->clingo->out_->predDoms().find(value.first.sig());
			if (it == ((ASPSolver*)this->solver)->clingo->out_->predDoms().end())
			{
				cout << "ASPFactsQuery: Didn't find any suitable domain!" << endl;
				continue;
			}
			for (int i = 0; i < (*it).get()->size(); i++)
			{
//				cout << "ASPFactsQuery: Inside domain-loop! " << *(*it) << endl;

//				if (&(domainPred)
//						&& (clingoModel)>isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
//				{
				if (clingoModel.contains(Gringo::Symbol((*(*it))[i])) && this->checkMatchValues(Gringo::Symbol(value.first), Gringo::Symbol((*(*it))[i])))
				{
					this->saveHeadValuePair(Gringo::Symbol(value.first), Gringo::Symbol((*(*it))[i]));
				}
//				}
			}
		}
	}

} /* namespace reasoner */

