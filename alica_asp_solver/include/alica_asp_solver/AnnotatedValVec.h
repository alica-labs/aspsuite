/*
 * AnnotatedValVec.h
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#ifndef SRC_ANNOTATEDVALVEC_H_
#define SRC_ANNOTATEDVALVEC_H_

#include <clingo/clingocontrol.hh>
#include <memory>
#include <alica_asp_solver/ASPQuery.h>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		class AnnotatedValVec
		{
		public:
//			AnnotatedValVec();
			AnnotatedValVec(long id, vector<Gringo::ValVec> values, shared_ptr<ASPQuery> query);
			virtual ~AnnotatedValVec();
			long id;
			vector<Gringo::ValVec> values;
			shared_ptr<ASPQuery> query;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ANNOTATEDVALVEC_H_ */
