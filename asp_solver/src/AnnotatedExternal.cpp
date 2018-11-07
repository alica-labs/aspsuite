/*
 * AnnotatedExternal.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: stefan
 */

#include "asp_solver/AnnotatedExternal.h"

namespace reasoner
{

	AnnotatedExternal::AnnotatedExternal(string aspPredicate, shared_ptr<Clingo::Symbol> gringoValue, bool value)
	{
		this->aspPredicate = aspPredicate;
		this->gringoValue = gringoValue;
		this->value = value;
	}

	AnnotatedExternal::~AnnotatedExternal()
	{
	}

	string AnnotatedExternal::getAspPredicate()
	{
		return this->aspPredicate;
	}

	void AnnotatedExternal::setAspPredicate(string aspPredicate)
	{
		this->aspPredicate = aspPredicate;
	}

	shared_ptr<Clingo::Symbol> AnnotatedExternal::getGringoValue()
	{
		return this->gringoValue;
	}

	void AnnotatedExternal::setGringoValue(shared_ptr<Clingo::Symbol> gringoValue)
	{
		this->gringoValue = gringoValue;
	}

	bool AnnotatedExternal::getValue()
	{
		return this->value;
	}

	void AnnotatedExternal::setValue(bool value)
	{
		this->value = value;
	}

} /* namespace reasoner */
