/*
 * AnnotatedValVec.h
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#ifndef SRC_COMMONS_ANNOTATEDVALVEC_H_
#define SRC_COMMONS_ANNOTATEDVALVEC_H_

#include <memory>
//#include <clingo/clingocontrol.hh>
#include <clingo.hh>

using namespace std;

namespace reasoner
{
	class ASPQuery;
	class AnnotatedValVec
	{
	public:
//			AnnotatedValVec();
		AnnotatedValVec(long id, vector<Clingo::SymbolVector> values, shared_ptr<ASPQuery> query);
		virtual ~AnnotatedValVec();
		long id;
		vector<vector<string>> variableQueryValues;
		vector<vector<string>> factQueryValues;
		shared_ptr<ASPQuery> query;
	private:
		vector<Clingo::SymbolVector> values;
		void extractResponse();
	};

} /* namespace reasoner */

#endif /* SRC_COMMONS_ANNOTATEDVALVEC_H_ */
