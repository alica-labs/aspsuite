/*
 * AspQuery.h
 *
 *  Created on: Jun 8, 2016
 *      Author: Stefan Jakob
 */

#ifndef SYMROCK_ASP_COMMONS_INCLUDE_ASP_COMMONS_ASPQUERY_H_
#define SYMROCK_ASP_COMMONS_INCLUDE_ASP_COMMONS_ASPQUERY_H_

#include <string>
#include <memory>
#include <map>
//#include <clingo/clingocontrol.hh>
#include <clingo.hh>
#include <asp_commons/ASPQueryType.h>

using namespace std;

//#define ASPQUERY_DEBUG

namespace reasoner
{

	class ASPCommonsTerm;
	class AnnotatedValVec;
	class IASPSolver;
	class ASPQuery
	{
	public:
		ASPQuery(IASPSolver* solver, shared_ptr<ASPCommonsTerm> term);
		virtual ~ASPQuery();

		shared_ptr<vector<Clingo::SymbolVector>> getCurrentModels();

		int getLifeTime();
		void setLifeTime(int lifeTime);
		void reduceLifeTime();

		virtual void onModel(Clingo::Model& clingoModel) = 0;

		map<Clingo::Symbol, Clingo::SymbolVector>& getHeadValues();

		IASPSolver* getSolver();
		vector<string> getRules();
		shared_ptr<ASPCommonsTerm> getTerm();

		string getProgrammSection();
		void setProgrammSection(string programmSection);
		virtual void removeExternal() = 0;

		string toString();
		ASPQueryType getType();
		void saveHeadValuePair(Clingo::Symbol key, Clingo::Symbol value);
		bool checkMatchValues(Clingo::Symbol value1, Clingo::Symbol value2);

	protected:
		/**
		 * queryString is used to ask the solver if specific predicates are true.
		 * predicates are separated by "," meaning all of them will be in the same rule and ";"
		 * meaning that there is a rule for every predicate
		 */
		IASPSolver* solver;
		shared_ptr<vector<Clingo::SymbolVector>> currentModels;
		vector<string> rules;
		// key := headValue , value := values which satisfies it
		map<Clingo::Symbol, Clingo::SymbolVector> headValues;
		// lifeTime == 1 => query is used once
		// lifeTime == x => query is used x times
		// LifeTime == -1 => query is used until unregistered
		int lifeTime;
		string programSection;
		shared_ptr<ASPCommonsTerm> term;
		ASPQueryType type;

	};

} /* namespace reasoner */

#endif /* SYMROCK_ASP_COMMONS_INCLUDE_ASP_COMMONS_ASPQUERY_H_ */
