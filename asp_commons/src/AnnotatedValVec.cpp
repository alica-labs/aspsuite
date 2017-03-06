/*
 * AnnotatedValVec.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#include "asp_commons/AnnotatedValVec.h"
#include "asp_commons/ASPQuery.h"
#include <Configuration.h>

namespace reasoner
{

	AnnotatedValVec::AnnotatedValVec(long id, vector<Gringo::SymVec> values, shared_ptr<ASPQuery> query)
	{
		this->id = id;
		this->values = values;
		this->query = query;
		extractResponse();
	}

	AnnotatedValVec::~AnnotatedValVec()
	{
	}

	void AnnotatedValVec::extractResponse()
	{
		for(int i = 0; i < values.size();i++)
		{
			this->variableQueryValues.push_back(vector<string>());
			this->factQueryValues.push_back(vector<string>());
			for(auto val : values.at(i))
			{
				stringstream ss;
				ss << val;
				string tmp = ss.str();
				tmp = supplementary::Configuration::trim(tmp);
				this->factQueryValues.at(i).push_back(tmp);
				tmp = tmp.substr(0, tmp.size() - 1);
				tmp = tmp.erase(0, tmp.find("(") + 1);
				this->variableQueryValues.at(i).push_back(tmp);
			}
		}
	}

} /* namespace reasoner */


