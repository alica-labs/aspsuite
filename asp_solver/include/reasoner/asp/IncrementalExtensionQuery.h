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
    static bool isPresent(int horizon);
    static void activate(int horizon);
    static void cleanUp();
    static void clear();
    static std::vector<IncrementalExtensionQuery*> queries;
    static int queryId;

    IncrementalExtensionQuery(int queryID, Solver* solver, Term* term);
    void onModel(Clingo::Model& clingoModel) override;
    void removeExternal() override;

private:
    void generateQueryProgram() override;
    std::string lastQuerySection;
};

}
}