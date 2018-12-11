#pragma once

#include <clingo.hh>
#include <iostream>

namespace reasoner
{
namespace asp
{
class GrdProgramObserver : public Clingo::GroundProgramObserver
{
public:
    GrdProgramObserver();

    virtual ~GrdProgramObserver();

    virtual void output_atom(Clingo::Symbol symbol, Clingo::atom_t atom) override;

    virtual void external(Clingo::atom_t atom, Clingo::ExternalType type) override;

    const std::string getGroundProgram() const;

    void clear();

private:
    std::string groundAtoms;
};
} /* namespace asp */
} /* namespace reasoner */
