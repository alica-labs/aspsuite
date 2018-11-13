#pragma once

#include <QObject>
#include <QString>

namespace kbcr
{

class ConceptNetConcept : public QObject
{
    Q_OBJECT
public:
    ConceptNetConcept(QString term, QString senseLabel, QString cn5ID);
    virtual ~ConceptNetConcept();

    QString term;
    QString senseLabel;
    QString cn5ID;
};

} /* namespace kbcr */

