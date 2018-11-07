#pragma once

#include <clingo.hh>

#include <string>
#include <vector>
#include <memory>

namespace reasoner
{
	class ASPCommonsTerm;
	class ASPCommonsVariable;
	class ASPQuery;
class IASPSolver : public std::enable_shared_from_this<IASPSolver>
	{
	public:
		static const void* const WILDCARD_POINTER;
		static const std::string WILDCARD_STRING;

		IASPSolver();
		virtual ~IASPSolver();

		virtual bool existsSolution(std::vector<std::shared_ptr<ASPCommonsVariable>>& vars, std::vector<std::shared_ptr<ASPCommonsTerm>>& calls) = 0;
		virtual bool getSolution(std::vector<std::shared_ptr<ASPCommonsVariable>>& vars, std::vector<std::shared_ptr<ASPCommonsTerm>>& calls,
								 std::vector<void*>& results) = 0;
		virtual std::shared_ptr<ASPCommonsVariable> createVariable(long id) = 0;

		virtual bool loadFileFromConfig(std::string configKey) = 0;
		virtual void loadFile(std::string filename) = 0;

		//virtual void ground(Clingo::GroundVec const &vec, Clingo::Context *context) = 0;
		virtual void ground(Clingo::PartSpan parts, Clingo::GroundCallback = nullptr) = 0; 
		virtual void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue) = 0;
		virtual void releaseExternal(Clingo::Symbol ext) = 0;
		virtual bool solve() = 0;
		virtual void add(char const *name, Clingo::StringSpan const &params, char const *par) = 0;
		virtual Clingo::Symbol parseValue(std::string const &str) = 0;
		virtual int getQueryCounter() = 0;

		virtual void removeDeadQueries() = 0;
		virtual bool registerQuery(std::shared_ptr<ASPQuery> query) = 0;
		virtual bool unregisterQuery(std::shared_ptr<ASPQuery> query) = 0;
		virtual void printStats() = 0;
		virtual std::vector<std::shared_ptr<ASPQuery>> getRegisteredQueries() = 0;

	};

} /* namespace reasoner */
