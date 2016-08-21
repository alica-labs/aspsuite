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

namespace alica
{
	namespace reasoner
	{

		class Term : public std::enable_shared_from_this<Term>, public alica::SolverTerm
		{
		public:
			Term(int lifeTime = 1);
			virtual ~Term();
			bool setRule(std::string rule);
			std::string getRuleHead();
			std::string getRuleBody();
			std::string getBackGroundFileName();
			void setBackgroundFileName(std::string backgroundFile);
			int getLifeTime();
			std::string getRule();
			void addFact(std::string fact);

		private :
			std::string rule;
			std::string head;
			std::string body;
			std::string backgroundFile;
			int lifeTime;
			std::vector<std::string> facts;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_TERM_H_ */
