/*
 * IASPSolver.h
 *
 *  Created on: Jan 26, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_ASP_COMMONS_IASPSOLVER_H_
#define INCLUDE_ASP_COMMONS_IASPSOLVER_H_

#include <string>
#include <vector>
#include <clingo/clingocontrol.hh>
#include <asp_commons/ASPVariable.h>
#include <asp_commons/ASPTerm.h>

using namespace std;

namespace reasoner
{
	class IASPSolver : public enable_shared_from_this<IASPSolver>
	{
	public:
		static const void* const WILDCARD_POINTER;
		static const string WILDCARD_STRING;

		IASPSolver();
		virtual ~IASPSolver() {};

		virtual bool existsSolution(vector<ASPVariable*>& vars, vector<shared_ptr<ASPTerm>>& calls) = 0;
		virtual bool getSolution(vector<ASPVariable*>& vars, vector<shared_ptr<ASPTerm>>& calls,
							vector<void*>& results) = 0;
		virtual shared_ptr<ASPVariable> createVariable(long id) = 0;

		virtual void disableWarnings(bool noWarns) = 0;
		virtual bool loadFileFromConfig(string configKey) = 0;
		virtual void loadFile(string filename) = 0;

		virtual void ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context) = 0;
		virtual void assignExternal(Gringo::Value ext, Gringo::TruthValue) = 0;
		virtual void releaseExternal(Gringo::Value ext) = 0;
		virtual bool solve() = 0;
		virtual void add(string const &name, Gringo::FWStringVec const &params, string const &par) = 0;
		virtual Gringo::Value parseValue(std::string const &str) = 0;
	};

} /* namespace reasoner */

#endif /* INCLUDE_ASP_COMMONS_IASPSOLVER_H_ */
