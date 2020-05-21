#pragma once

#include "ExtensionQuery.h"

namespace reasoner
{
namespace asp
{
class Solver;
class Term;
class IncrementalExtensionQuery : public ::reasoner::asp::ExtensionQuery
{

public:

    int queryId;


    IncrementalExtensionQuery(Solver* solver, Term* term, const std::string& queryExternalPrefix, int horizon);
    void onModel(Clingo::Model& clingoModel) override;
    void removeExternal() override;

private:
    void createProgramSection() override;
    std::string lastQuerySection;
};

}
}