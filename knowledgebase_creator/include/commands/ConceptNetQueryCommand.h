#pragma once

#include <commands/Command.h>

#include <memory>
#include <string>

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

#include <chrono>
#include <fstream>

//#define ConceptNetQueryCommandDebug
//#define CNQC_EVALCODE

namespace kbcr
{

class ConceptNetCall;
class ConceptNetEdge;
class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to create a ConceptNet Query
 */
class ConceptNetQueryCommand : public Command, public std::enable_shared_from_this<ConceptNetQueryCommand>
{
    Q_OBJECT
public:
    ConceptNetQueryCommand(KnowledgebaseCreator* gui, QString query);
    virtual ~ConceptNetQueryCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString query;
    /**
     * is wildcard in case of wildcard(concept, concept) since its not clear which one is ment
     */
    QString queryConcept;
    /**
     * Encapsulates the results given from COncept Net
     */
    ConceptNetCall* currentConceptNetCall;

private slots:
    /**
     * Slot accepting the http request from concept net
     */
    void conceptNetCallFinished(QNetworkReply* reply);
    /**
     * Slot used to parse the result given JSON to asp conform syntax
     */
    void extractASPPredicates();

private:
    QString expandConceptNetPredicate(QString predicate);
    /**
     * Handles the http requests
     */
    QNetworkAccessManager* nam;
    /**
     * Extracts ASP predicates from concept net answer
     */
    QString createASPPredicates();
    /**
     * removes dots, commas and spaces from concept net answer
     */
    QString conceptToASPPredicate(QString concept);
    /**
     * Checks if query is one of the concept net base relations
     */
    bool isConceptNetRelation(QString query);
    /**
     * Informs the user that the given query is not supported by concept net
     */
    void handleWrongInput();
    /**
     * Handles the 3 query types:
     * 1: plain concept
     * 2: relation(concept, wildcard)
     * 3: relation(wildcard, concept)
     * 4: wildcard(concept, concept)
     * 5: relation(concept)
     */
    void handleQuery();
    /**
     * Removes leading /c/en/ from a concept net term representing a concept
     */
    QString trimTerm(QString term);
    /**
     * Executes http get request
     */
    void callUrl(QUrl url);
    /**
     * Map of metaknowlege relation to asp progsm containing this relation
     */
    std::map<QString, QString> extractBackgroundKnowledgePrograms(QString conceptNetProgram);
    QString createBackgroundKnowledgeRule(QString relation, std::shared_ptr<ConceptNetEdge> edge);
    QString conceptWithoutPrefix;
    bool conceptContainsUTF8(QString concept);

    bool minWeightPassed;

    int prefixLength;

#ifdef CNQC_EVALCODE
    std::chrono::_V2::system_clock::time_point start;
    std::ofstream outFS;
#endif

signals:
    /**
     * Signal used as soon as the extraction from concept net is finished
     */
    void jsonExtracted();
};

} /* namespace kbcr */
