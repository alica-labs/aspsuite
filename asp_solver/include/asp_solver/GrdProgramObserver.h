#pragma once

#include <clingo.hh>
#include <iostream>

class GrdProgramObserver : public Clingo::GroundProgramObserver{
public:
	virtual void output_atom(Clingo::Symbol symbol, Clingo::atom_t atom) override {
		groundProgram.append(symbol.to_string()).append(" ");
	}

	const std::string getGroundProgram() const {
		return groundProgram;
	}

	void clear() {
		this->groundProgram.clear();
	}

private:
	std::string groundProgram = "";
};


/*
bla(X) :- test(X).
test(1).
*/
