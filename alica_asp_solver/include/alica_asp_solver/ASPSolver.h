/*
 * ASPSolver.h
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVER_H_
#define SRC_ASPSOLVER_H_

#include <alica_asp_solver/ASPQuery.h>
#include <alica_asp_solver/ASPFactsQuery.h>
#include <alica_asp_solver/ASPVariableQuery.h>
#include "ASPAlicaPlanIntegrator.h"
#include "ASPGenerator.h"
#include <clingo/clingocontrol.hh>
#include <engine/constraintmodul/ISolver.h>
#include <SystemConfig.h>
#include "engine/model/Variable.h"
#include "AnnotatedExternal.h"
#include <mutex>

#include <memory>
#include <vector>

//#define ASPSolver_DEBUG
//#define ASP_TEST_RELATED
//#define SOLVER_OPTIONS

using namespace std;

namespace alica
{
	class AlicaEngine;
	class Plan;

	namespace reasoner
	{

		class ASPSolver : public alica::ISolver
		{
		public:
			static const void* const WILDCARD_POINTER;
			static const string WILDCARD_STRING;

			ASPSolver(AlicaEngine* ae, std::vector<char const*> args);
			virtual ~ASPSolver();

			bool existsSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls);
			bool getSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ProblemDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);

			bool validatePlan(Plan* plan);
			void disableWarnings(bool noWarns);
			bool loadFileFromConfig(string configKey);
			void loadFile(string filename);

			void ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context);
			bool onModel(Gringo::Model const &m);
			void assignExternal(Gringo::Value ext, Gringo::TruthValue);
			void releaseExternal(Gringo::Value ext);
			bool solve();
			void add(string const &name, Gringo::FWStringVec const &params, string const &par);
			Gringo::Value parseValue(std::string const &str);

			bool registerQuery(shared_ptr<ASPQuery> query);
			bool unregisterQuery(shared_ptr<ASPQuery> query);
			int getRegisteredQueriesCount();
			/**
			 * The query id has to be added to any predicate in an ASPTerm with type Variable
			 * which is added to the program, naming rule heads and facts!
			 * An unique id is given by the ASPSolver!
			 */
			int getQueryCounter();
			void removeDeadQueries();

			const long long getSolvingTime();
			const long long getSatTime();
			const long long getUnsatTime();
			const long getModelCount();
			const long getAtomCount();
			const long getBodiesCount();
			const long getAuxAtomsCount();
			void printStats();

			alica::reasoner::ASPGenerator gen;
			static const void* getWildcardPointer();
			static const string& getWildcardString();

		private:
			shared_ptr<ClingoLib> clingo;
			DefaultGringoModule* gringoModule;
			Gringo::ConfigProxy* conf;
			unsigned int root;
			unsigned int modelsKey;

			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
			vector<string> alreadyLoaded;
			vector<shared_ptr<AnnotatedExternal>> assignedExternals;
			vector<shared_ptr<ASPQuery>> registeredQueries;

			void reduceQueryLifeTime();
			void integrateRules();
			int prepareSolution(std::vector<alica::Variable*>& vars,
								std::vector<shared_ptr<ProblemDescriptor> >& calls);
			int queryCounter;
			supplementary::SystemConfig* sc;

		protected:
			static mutex queryCounterMutex;

#ifdef ASPSolver_DEBUG
			int modelCount;
#endif
		};
		template <class T>
		void traverseOptions(T& conf, unsigned key, std::string accu) {
		   int subKeys, arrLen;
		   const char* help;
		   conf.getKeyInfo(key, &subKeys, &arrLen, &help);
		   if (arrLen > 0) {
		     for (int i = 0; i != arrLen; ++i) {
		       traverseOptions(conf, conf.getArrKey(key, i), accu +
		std::to_string(i) + ".");
		     }
		   }
		   else if (subKeys > 0) {
		     for (int i = 0; i != subKeys; ++i) {
		       const char* sk = conf.getSubKeyName(key, i);
		       traverseOptions(conf, conf.getSubKey(key, sk), accu + sk);
		     }
		   }
		   else {
		     std::cout << accu << " -- " << (help ? help : "") << "\n";
		   }
		}

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVER_H_ */
