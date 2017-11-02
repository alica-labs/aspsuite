/*
 * Term.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ASP_WRAPPER_TERM_H_
#define INCLUDE_ASP_WRAPPER_TERM_H_

#include <engine/constraintmodul/SolverTerm.h>
#include <asp_commons/ASPQueryType.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class ASPTerm : public enable_shared_from_this<ASPTerm>, public alica::SolverTerm
		{
		public:
			ASPTerm(int lifeTime = 1);
			virtual ~ASPTerm();
			void addRule(string rule);
			vector<string> getRuleHeads();
			vector<string> getRuleBodies();
			string getProgramSection();
			void setProgramSection(string programSection);
			int getLifeTime();
			vector<string> getRules();
			void addFact(string fact);
			vector<string> getFacts();
			void setExternals(shared_ptr<map<string, bool>> externals);
			shared_ptr<map<string, bool> > getExternals();
			string getNumberOfModels();
			void setNumberOfModels(string numberOfModels);
			::reasoner::ASPQueryType getType();
			void setType(::reasoner::ASPQueryType type);
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
			::reasoner::ASPQueryType type;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ASP_WRAPPER_TERM_H_ */
