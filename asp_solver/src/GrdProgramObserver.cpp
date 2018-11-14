#include "asp_solver/GrdProgramObserver.h"

namespace reasoner
{

GrdProgramObserver::GrdProgramObserver()
{
	this->atomCounter = 1;
}

GrdProgramObserver::~GrdProgramObserver() {}

const std::string GrdProgramObserver::getGroundProgram() const {
	std::string groundProgram = "";
	for(auto rule : rules) {
		for(int i = 0; i < rule->first->size(); i++) {
			groundProgram.append(this->idSymbolMapping.at(rule->first->at(i)));
			if(rule->first->size() > 1 && i < rule->first->size()) {
				groundProgram.append(", ");
			}
		}
		if(rule->second->size() > 0) {
			groundProgram.append(" :- ");
			for(int i = 0; i < rule->second->size(); i++) {
				groundProgram.append(this->idSymbolMapping.at(rule->second->at(i)));
				if(rule->second->size() > 1 && i < rule->second->size()) {
					groundProgram.append(", ");
				}
			}
		}
		groundProgram.append(".\n");
	}
	return groundProgram;
}

void GrdProgramObserver::output_atom(Clingo::Symbol symbol, Clingo::atom_t atom) {
	std::cout << symbol << std::endl;
	this->idSymbolMapping.emplace(this->atomCounter, symbol.to_string());
	this->atomCounter++;
}

void GrdProgramObserver::rule(bool choice, Clingo::AtomSpan head, Clingo::LiteralSpan body) {
	std::cout << head << " :- " << body << std::endl;
	auto headVector = new std::vector<unsigned int>;
	for(auto atom : head) {
		headVector->push_back((unsigned int)atom);
	}
	auto bodyVector = new std::vector<unsigned int>;
	for(auto atom : body) {
		bodyVector->push_back((unsigned int)atom);
	}
	this->rules.push_back(new std::pair<std::vector<unsigned int>*, std::vector<unsigned int>*>(headVector, bodyVector));
}

void GrdProgramObserver::output_term(Clingo::Symbol symbol, Clingo::LiteralSpan condition) {
	std::cout << "Term: " << symbol << " " << condition << std::endl;
}

void GrdProgramObserver::external(Clingo::atom_t atom, Clingo::ExternalType type) {
	std::cout << "External: " << atom << " " << type << std::endl;
}

void GrdProgramObserver::clear() {
	for(auto pair : this->rules) {
		delete pair->first;
		delete pair->second;
		delete pair;
	}
	this->rules.clear();
	this->idSymbolMapping.clear();
	this->atomCounter = 1;
}

void GrdProgramObserver::setClingo(std::shared_ptr<Clingo::Control> clingo) {
	this->clingo = clingo;
}

}

/*
a(X), c(X) :- b(X).
b(1).
b(2).
b(3).
 */
