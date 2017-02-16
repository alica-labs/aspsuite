/*
 * AnnotatedValVec.h
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#ifndef SRC_COMMONS_ANNOTATEDVALVEC_H_
#define SRC_COMMONS_ANNOTATEDVALVEC_H_

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
		vector<vector<string>> parsedValues;
		shared_ptr<ASPQuery> query;
	private:
		void removeQueryExpansion();
	};

} /* namespace reasoner */

#endif /* SRC_COMMONS_ANNOTATEDVALVEC_H_ */
