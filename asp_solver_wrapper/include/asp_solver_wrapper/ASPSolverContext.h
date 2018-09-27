#pragma once

#include <alica_solver_interface/SolverContext.h>
#include <memory>
#include <vector>

namespace alica
{
    namespace reasoner
    {
        class ASPVariable;

        class ASPSolverContext : public SolverContext
        {
        public:
            ASPVariable* createVariable(int64_t id);
            virtual void clear() override;
            const std::vector<std::unique_ptr<ASPVariable>>& getVariables() const { return _vars; }

        private:
            std::vector<std::unique_ptr<ASPVariable>> _vars;
        };
    }
}
