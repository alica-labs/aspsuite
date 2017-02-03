/*
 * Term.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_TERM_H_
#define INCLUDE_ALICA_ASP_SOLVER_TERM_H_

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../include/asp_commons/ASPQueryType.h"

using namespace std;

namespace reasoner
{

	class ASPCommonsTerm : public enable_shared_from_this<ASPCommonsTerm>
	{
	public:
		ASPCommonsTerm(int lifeTime = 1);
		virtual ~ASPCommonsTerm();
		void addRule(string rule);
		vector<string> getRuleHeads();
		vector<string> getRuleBodies();
		string getProgramSection();
		void setProgramSection(string programSection);
		int getLifeTime();
		void setLifeTime(int lifeTime);
		vector<string> getRules();
		void addFact(string fact);
		vector<string> getFacts();
		void setExternals(shared_ptr<map<string, bool>> externals);
		shared_ptr<map<string, bool> > getExternals();
		string getNumberOfModels();
		void setNumberOfModels(string numberOfModels);
		ASPQueryType getType();
		void setType(ASPQueryType type);
		long getId();
		void setId(long id);
		int getQueryId();
		void setQueryId(int queryId);
		string getQueryRule();
		void setQueryRule(string queryRule);

	private:
		string numberOfModels;
		string queryRule;
		vector<string> rules;
		vector<string> heads;
		vector<string> bodies;
		string programSection;
		long id;
		/**
		 * The query id has to be added to any predicate which is added to the program, naming rule
		 * heads and facts!
		 * An unique id is given by the ASPSolver!
		 */
		int queryId;
		int lifeTime;
		vector<string> facts;
		shared_ptr<map<string, bool>> externals;
		ASPQueryType type;
	};

} /* namespace reasoner */

#endif /* INCLUDE_ALICA_ASP_SOLVER_TERM_H_ */
