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

#include <memory>
#include <vector>

#define ASPSolver_DEBUG

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

			bool existsSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls);
			bool getSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor>>& calls,
								vector<void*>& results);
			shared_ptr<SolverVariable> createVariable(long id);
			bool validatePlan(Plan* plan);

			void disableWarnings(bool noWarns);
			void load(string filename);
			void ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context);
			vector<Gringo::Value> createQueryValues(std::string const &queryString);
			bool isTrue(Gringo::Value queryValue);
			bool isTrueForAtLeastOneModel(const string& queryValue);
			bool isTrueForAllModels(const string& queryValue);
			std::vector<Gringo::Value> getAllMatches(Gringo::Value queryValue);
			bool onModel(Gringo::Model const &m);
			bool solve();
			bool registerQuery(const string& query);
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

		private:
			shared_ptr<ClingoLib> clingo;
			DefaultGringoModule gringoModule;
			shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
			vector<Gringo::ValVec> currentModels;



			// key=queries, value=vector<true predicates in the last model>
			map<Gringo::Value, vector<Gringo::Value>> registeredQueries;
			vector<shared_ptr<AspQuery>> regQueries;

			bool checkMatchValues(const Gringo::Value* value1, const Gringo::Value* value2);
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* SRC_ASPSOLVER_H_ */
