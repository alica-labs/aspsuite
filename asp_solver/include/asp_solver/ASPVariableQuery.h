#pragma once
#include <clingo.hh>

#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <memory>

//#define ASPVARIABLEQUERY_DEBUG

namespace reasoner
{
	class ASPSolver;
	class ASPVariableQuery : public ASPQuery
	{
	public:
		ASPVariableQuery(ASPSolver* solver, std::shared_ptr<reasoner::ASPCommonsTerm> term);
		virtual ~ASPVariableQuery();
		std::vector<std::string> getRules();
		ASPQueryType getType();
		void removeExternal();
		void onModel(Clingo::Model& clingoModel);

	private:
		void createHeadQueryValues(std::vector<std::string> queryVec);
		std::vector<std::string> createHeadPredicates(std::string head);
		std::string expandQueryRuleModuleProperty(std::string rule);
		std::string expandRuleModuleProperty(std::string rule);
		std::string expandFactModuleProperty(std::string fact);
		void replaceFittingPredicate(std::string& ruleBody, std::string predicate);
		std::string replaceHeadPredicates(std::string head);
		std::shared_ptr<Clingo::Symbol> external;
		std::string queryProgramSection;
		std::string externalName;
		void createProgramSection();
	};

} /* namespace reasoner */
