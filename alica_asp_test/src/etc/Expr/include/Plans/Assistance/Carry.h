#ifndef Carry_H_
#define Carry_H_

#include "DomainBehaviour.h"
#include <engine/constraintmodul/Query.h>
/*PROTECTED REGION ID(inc1463077939671) ENABLED START*/ // Add additional includes here
/*PROTECTED REGION END*/
namespace alica
{
class Carry : public DomainBehaviour
{
public:
    Carry();
    virtual ~Carry();
    virtual void run(void* msg);
    /*PROTECTED REGION ID(pub1463077939671) ENABLED START*/ // Add additional public methods here
    /*PROTECTED REGION END*/
protected:
    virtual void initialiseParameters();
    /*PROTECTED REGION ID(pro1463077939671) ENABLED START*/ // Add additional protected methods here
    shared_ptr<alica::Query> query;
    /*PROTECTED REGION END*/
private:
/*PROTECTED REGION ID(prv1463077939671) ENABLED START*/ // Add additional private methods here
        /*PROTECTED REGION END*/};
        } /* namespace alica */

#endif /* Carry_H_ */
