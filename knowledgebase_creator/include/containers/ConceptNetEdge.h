#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QString>

namespace kbcr
{

/**
 * Class representing an edge of the concept net graph
 */
class ConceptNetConcept;
class ConceptNetEdge
{
public:
    ConceptNetEdge(QString id, QString language, std::shared_ptr<ConceptNetConcept> firstConcept, std::shared_ptr<ConceptNetConcept> secondConcept,
            QString relation, double weight);
    virtual ~ConceptNetEdge();

    QString id;
    QString language;
    std::shared_ptr<ConceptNetConcept> firstConcept;
    std::shared_ptr<ConceptNetConcept> secondConcept;
    QString relation;
    double weight;
    std::vector<QString> sources;

    std::string toString();
};

} /* namespace kbcr */

