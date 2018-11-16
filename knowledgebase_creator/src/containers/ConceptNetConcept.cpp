#include "containers/ConceptNetConcept.h"

namespace kbcr
{

ConceptNetConcept::ConceptNetConcept(QString term, QString senseLabel, QString cn5ID)
{
    this->term = term;
    this->senseLabel = senseLabel;
    this->cn5ID = cn5ID;
}

ConceptNetConcept::~ConceptNetConcept() {}

} /* namespace kbcr */
