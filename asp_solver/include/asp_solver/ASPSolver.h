/*
 * ASPSolver.h
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVER_H_
#define SRC_ASPSOLVER_H_

//#include <clingo/clingocontrol.hh>
#include <clingo.hh>
#include <SystemConfig.h>
#include <mutex>

#include <memory>
#include <vector>

#include <asp_commons/IASPSolver.h>

//#define ASPSolver_DEBUG
//#define ASP_TEST_RELATED
//#define SOLVER_OPTIONS

using namespace std;

namespace reasoner
{

	class AnnotatedExternal;
	class ASPFactsQuery;
	class ASPQuery;
	class ASPVariableQuery;
	class ASPCommonsVariable;
	class AnnotatedValVec;
	class ASPSolver : public IASPSolver, Clingo::SolveEventHandler
	{
	public:
		ASPSolver(std::vector<char const*> args);
		virtual ~ASPSolver();

		bool existsSolution(vector<shared_ptr<ASPCommonsVariable>>& vars, vector<shared_ptr<ASPCommonsTerm>>& calls);
		bool getSolution(vector<shared_ptr<ASPCommonsVariable>>& vars, vector<shared_ptr<ASPCommonsTerm>>& calls,
							vector<void*>& results);
		shared_ptr<ASPCommonsVariable> createVariable(long id);

		bool loadFileFromConfig(string configKey);
		void loadFile(string filename);

		void ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack);
		void assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue);
		void releaseExternal(Clingo::Symbol ext);
		bool solve();
		void add(char const *name, Clingo::StringSpan const &params, char const *par);
		Clingo::Symbol parseValue(std::string const &str);

		int getRegisteredQueriesCount();
		/**
		 * The query id has to be added to any predicate in an ASPTerm with type Variable
		 * which is added to the program, naming rule heads and facts!
		 * An unique id is given by the ASPSolver!
		 */
		int getQueryCounter();

		void removeDeadQueries();
		bool registerQuery(shared_ptr<ASPQuery> query);
		bool unregisterQuery(shared_ptr<ASPQuery> query);
		void printStats();

		const double getSolvingTime();
		const double getSatTime();
		const double getUnsatTime();
		const double getModelCount();
		const double getAtomCount();
		const double getBodiesCount();
		const double getAuxAtomsCount();

		static const void* getWildcardPointer();
		static const string& getWildcardString();
		vector<shared_ptr<ASPQuery>> getRegisteredQueries();
		vector<Clingo::SymbolVector> getCurrentModels();
		//DefaultGringoModule* getGringoModule();

		shared_ptr<Clingo::Control> clingo;

	private:
		bool onModel(Clingo::Model &m);
//		Gringo::Control* clingo;
		//DefaultGringoModule* gringoModule;
		//unsigned int root;
		vector<long> currentQueryIds;

		vector<string> alreadyLoaded;
		vector<shared_ptr<AnnotatedExternal>> assignedExternals;
		vector<shared_ptr<ASPQuery>> registeredQueries;
		vector<Clingo::SymbolVector> currentModels;

		void reduceQueryLifeTime();
		int prepareSolution(std::vector<shared_ptr<ASPCommonsVariable>>& vars, vector<shared_ptr<ASPCommonsTerm>>& calls);
		int queryCounter;
		supplementary::SystemConfig* sc;

	protected:
		static mutex queryCounterMutex;

#ifdef ASPSolver_DEBUG
		int modelCount;
#endif
	};
	template<class T>
	void traverseOptions(T& conf, unsigned key, std::string accu)
	{
		int subKeys, arrLen;
		const char* help;
		conf->getKeyInfo(key, &subKeys, &arrLen, &help);
		if (arrLen > 0)
		{
			for (int i = 0; i != arrLen; ++i)
			{
				traverseOptions(conf, conf->getArrKey(key, i), accu + std::to_string(i) + ".");
			}
		}
		else if (subKeys > 0)
		{
			for (int i = 0; i != subKeys; ++i)
			{
				const char* sk = conf->getSubKeyName(key, i);
				traverseOptions(conf, conf->getSubKey(key, sk), accu + sk);
			}
		}
		else
		{
			std::cout << accu << " -- " << (help ? help : "") << "\n";
		}
	}

} /* namespace reasoner */

#endif /* SRC_ASPSOLVER_H_ */
