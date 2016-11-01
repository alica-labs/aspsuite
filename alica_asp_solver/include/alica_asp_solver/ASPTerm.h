/*
 * Term.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_TERM_H_
#define INCLUDE_ALICA_ASP_SOLVER_TERM_H_

#include <engine/constraintmodul/SolverTerm.h>
#include "ASPQueryType.h"
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
			bool setRule(string rule);
			string getRuleHead();
			string getRuleBody();
			string getProgrammSection();
			void setProgrammSection(string programmSection);
			int getLifeTime();
			string getRule();
			void addFact(string fact);
			vector<string> getFacts();
			void setExternals(shared_ptr<map<string, bool>> externals);
			shared_ptr<map<string, bool> > getExternals();
			string getNumberOfModels();
			void setNumberOfModels(string numberOfModels);
			ASPQueryType getType();
			void setType(ASPQueryType type);

		private:
			string numberOfModels;
			string rule;
			string head;
			string body;
			string programmSection;
			int lifeTime;
			vector<string> facts;
			shared_ptr<map<string, bool>> externals;
			ASPQueryType type;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_TERM_H_ */
