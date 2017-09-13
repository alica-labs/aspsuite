/*
 * IASPSolver.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: stefan
 */

#include <asp_commons/IASPSolver.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>

namespace reasoner
{
	const void* const IASPSolver::WILDCARD_POINTER = new int(0);
	const string IASPSolver::WILDCARD_STRING = "wildcard";

	IASPSolver::IASPSolver()
	{

	}

	IASPSolver::~IASPSolver()
	{
	}

} /* namespace cng */
