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
    static void cleanUp();
    static bool isPresent(int horizon);
    static void activate(int horizon);
    static std::vector<IncrementalExtensionQuery*> queries;
    static int queryId;


    IncrementalExtensionQuery(Solver* solver, Term* term);
    void onModel(Clingo::Model& clingoModel) override;
    void removeExternal() override;

private:
    void createProgramSection() override;
    std::string lastQuerySection;
};

}
}