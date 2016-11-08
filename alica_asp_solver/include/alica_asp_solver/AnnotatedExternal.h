/*
 * AnnotatedExternal.h
 *
 *  Created on: Oct 27, 2016
 *      Author: stefan
 */

#ifndef SRC_ANNOTATEDEXTERNAL_H_
#define SRC_ANNOTATEDEXTERNAL_H_

#include <clingo/clingocontrol.hh>
#include <memory>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class AnnotatedExternal
		{
		public:
			AnnotatedExternal(string aspPredicate, shared_ptr<Gringo::Value> gringoValue, bool value);
			virtual ~AnnotatedExternal();
			string getAspPredicate();
			void setAspPredicate(string aspPredicate);
			shared_ptr<Gringo::Value> getGringoValue();
			void setGringoValue(shared_ptr<Gringo::Value> gringoValue);
			bool getValue();
			void setValue(bool value);


		private:
			string aspPredicate;
			shared_ptr<Gringo::Value> gringoValue;
			bool value;


		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ANNOTATEDEXTERNAL_H_ */
