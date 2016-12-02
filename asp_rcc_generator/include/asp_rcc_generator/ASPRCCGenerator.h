/*
 * ASPRCCGenerator.h
 *
 *  Created on: Dec 1, 2016
 *      Author: stefan
 */

#ifndef INCLUDE_ASP_RCC_GENERATOR_ASPRCCGENERATOR_H_
#define INCLUDE_ASP_RCC_GENERATOR_ASPRCCGENERATOR_H_

#include <string>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class ASPRCCGenerator
		{
		public:
			ASPRCCGenerator(int endCount, int step, int percentageStep);
			virtual ~ASPRCCGenerator();
			void generateLPFile();

		private:
			int endCount;
			int step;
			int percentageStep;
			std::string createRandomConnections(int size, int percentage);
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ASP_RCC_GENERATOR_ASPRCCGENERATOR_H_ */
