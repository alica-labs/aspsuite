#pragma once

#include <clingo.hh>
#include <iostream>

#include <map>
#include <vector>
#include <memory>

namespace reasoner
{
class GrdProgramObserver : public Clingo::GroundProgramObserver{
public:

	GrdProgramObserver();
	virtual ~GrdProgramObserver();

	virtual void output_atom(Clingo::Symbol symbol, Clingo::atom_t atom) override;

	virtual void rule(bool choice, Clingo::AtomSpan head, Clingo::LiteralSpan body) override;

	virtual void output_term(Clingo::Symbol symbol, Clingo::LiteralSpan condition) override;

	virtual void external(Clingo::atom_t atom, Clingo::ExternalType type) override;

	const std::string getGroundProgram() const;

	void clear();

	void setClingo(std::shared_ptr<Clingo::Control> clingo);

private:
	std::map<unsigned int, std::string> idSymbolMapping;
	std::vector<std::pair<std::vector<unsigned int>*, std::vector<unsigned int>*>*> rules;
	unsigned int atomCounter;
	std::shared_ptr<Clingo::Control> clingo;

};
}
