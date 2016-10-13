/*
 * ASPSolver.h
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVER_H_
#define SRC_ASPSOLVER_H_

#include "ASPAlicaPlanIntegrator.h"
#include "ASPGenerator.h"
#include "AspQuery.h"
#include <engine/constraintmodul/IConstraintSolver.h>
#include <clingo/clingocontrol.hh>
#include <SystemConfig.h>
#include "engine/model/Variable.h"

#include <memory>
#include <vector>

#define ASPSolver_DEBUG

using namespace std;

namespace alica
{
	class AlicaEngine;
	class Plan;

	namespace reasoner
	{

		class ASPSolver : public alica::IConstraintSolver
		{
		public:
			static const void* const WILDCARD_POINTER;
			static const string WILDCARD_STRING;

			ASPSolver(AlicaEngine* ae, std::vector<char const*> args);
			virtual ~ASPSolver();

			bool existsSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls);
			bool getSolution(vector<alica::Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);

			bool validatePlan(Plan* plan);
			void disableWarnings(bool noWarns);
			void load(string filename);
			void loadFromConfig(string filename);
			bool loadFromConfigIfNotYetLoaded(string filename);
			void ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context);
			vector<Gringo::Value> createQueryValues(string const &queryString);
			bool isTrueForAtLeastOneModel(shared_ptr<AspQuery> query);
			bool isTrueForAllModels(shared_ptr<AspQuery> query);
			std::vector<Gringo::Value> getAllMatches(Gringo::Value queryValue);
			bool onModel(Gringo::Model const &m);
			bool solve();
			bool registerQuery(shared_ptr<AspQuery> query);
			bool unRegisterQuery(shared_ptr<AspQuery> query);
			alica::reasoner::ASPGenerator gen;


			const long long getSolvingTime();
			const long long getSatTime();
			const long long getUnsatTime();
			const long getModelCount();
			const long getAtomCount();
			const long getBodiesCount();
			const long getAuxAtomsCount();

			void printStats();
			DefaultGringoModule* getGringoModule();
			static const void* getWildcardPointer();
			static const string& getWildcardString();
			vector<shared_ptr<AspQuery> > getRegisteredQueries();
			shared_ptr<ClingoLib> getClingo();
			int getQueryCounter();
			void removeDeadQueries();

		private:
			shared_ptr<ClingoLib> clingo;
			DefaultGringoModule* gringoModule;
			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
			vector<Gringo::ValVec> currentModels;
			vector<string> alreadyLoaded;

			// key=queries, value=vector<true predicates in the last model>
			//map<Gringo::Value, vector<Gringo::Value>> registeredQueries;
			vector<shared_ptr<AspQuery>> registeredQueries;

			bool checkMatchValues(const Gringo::Value* value1, const Gringo::Value* value2);
			void reduceLifeTime();
			void integrateRules();
			int prepareSolution(std::vector<alica::Variable*>& vars,
								std::vector<shared_ptr<ConstraintDescriptor> >& calls);

			int queryCounter;
			supplementary::SystemConfig* sc;
#ifdef ASPSolver_DEBUG
			int modelCount;
#endif
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVER_H_ */
