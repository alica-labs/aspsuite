/*
 * Term.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_TERM_H_
#define INCLUDE_ALICA_ASP_SOLVER_TERM_H_

#include <engine/constraintmodul/SolverTerm.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class Term : public enable_shared_from_this<Term>, public alica::SolverTerm
		{
		public:
			Term(int lifeTime = 1);
			virtual ~Term();
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

		private :
			string numberOfModels;
			string rule;
			string head;
			string body;
			string programmSection;
			int lifeTime;
			vector<string> facts;
			shared_ptr<map<string, bool>> externals;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_TERM_H_ */
