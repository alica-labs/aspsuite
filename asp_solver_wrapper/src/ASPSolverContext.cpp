#include "asp_solver_wrapper/ASPSolverContext.h"
#include "asp_solver_wrapper/ASPVariable.h"

namespace alica
{
    namespace reasoner
    {
        ASPVariable* ASPSolverContext::createVariable(int64_t id)
        {
            ASPVariable* dv = new ASPVariable(id);
            _vars.emplace_back(dv);
            return dv;
        }
        void ASPSolverContext::clear()
        {
            _vars.clear();
        }
    }
}