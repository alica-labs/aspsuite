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

using namespace std;

namespace reasoner
{
	class ASPQuery;
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

#endif /* SRC_ANNOTATEDVALVEC_H_ */
