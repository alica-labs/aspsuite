/*
 * AnnotatedExternal.h
 *
 *  Created on: Oct 27, 2016
 *      Author: stefan
 */

#ifndef SRC_ANNOTATEDEXTERNAL_H_
#define SRC_ANNOTATEDEXTERNAL_H_

//#include <clingo/clingocontrol.hh>
#include <clingo.hh>
#include <memory>

using namespace std;

namespace reasoner
{

class AnnotatedExternal
{
public:
    AnnotatedExternal(string aspPredicate, shared_ptr<Clingo::Symbol> gringoValue, bool value);
    virtual ~AnnotatedExternal();
    string getAspPredicate();
    void setAspPredicate(string aspPredicate);
    shared_ptr<Clingo::Symbol> getGringoValue();
    void setGringoValue(shared_ptr<Clingo::Symbol> gringoValue);
    bool getValue();
    void setValue(bool value);

private:
    string aspPredicate;
    shared_ptr<Clingo::Symbol> gringoValue;
    bool value;
};

} /* namespace reasoner */
#endif /* SRC_ANNOTATEDEXTERNAL_H_ */
