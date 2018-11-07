#pragma once

#include <clingo.hh>

#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPTruthValue.h>

namespace reasoner
{

	class ASPSolver;
	class ASPFactsQuery : public ASPQuery
	{
	public:
		ASPFactsQuery(ASPSolver* solver, std::shared_ptr<ASPCommonsTerm> term);
		virtual ~ASPFactsQuery();
		std::map<Clingo::Symbol, std::vector<Clingo::Symbol> > getFactModelMap();
		void setFactModelMap(std::map<Clingo::Symbol, std::vector<Clingo::Symbol> > factModelMap);
		std::shared_ptr<std::map<Clingo::Symbol, std::vector<Clingo::Symbol>>> getSatisfiedFacts();
		void saveSatisfiedFact(Clingo::Symbol key, Clingo::Symbol value);
		bool factsExistForAllModels();
		bool factsExistForAtLeastOneModel();
		void removeExternal();
		std::vector<std::pair<Clingo::Symbol, ASPTruthValue>> getASPTruthValues();
		void onModel(Clingo::Model& clingoModel);

	private:
		std::vector<Clingo::Symbol> queryValues;
		void createQueryValues(std::vector<std::string> queryString);
		// key := query value, value := predicates that satisfy the query value
		std::map<Clingo::Symbol, std::vector<Clingo::Symbol>> factModelMap;
	};

}
/* namespace reasoner */
