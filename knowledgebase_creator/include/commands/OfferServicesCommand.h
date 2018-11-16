#pragma once

#include "commands/Command.h"

#include "containers/ConceptNetCall.h"

#include <chrono>
#include <fstream>
#include <memory>

namespace kbcr
{

class KnowledgebaseCreator;
class OfferServicesCommand : public Command, public std::enable_shared_from_this<OfferServicesCommand>
{
    Q_OBJECT
public:
    OfferServicesCommand(KnowledgebaseCreator* gui, QString queryString);
    virtual ~OfferServicesCommand();

    void execute();
    /**
     * Undo the Command
     */
    void undo();

    /**
     * Creates JSON String to be saved
     * @return QJsonObject
     */
    QJsonObject toJSON();
    KnowledgebaseCreator* gui;
    QString queryString;
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
     * Informs the user that the given query is not supported by concept net
     */
    void handleWrongInput();
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
    bool conceptContainsUTF8(QString concept);

    bool minWeightPassed;

    int prefixLength;

    std::chrono::_V2::system_clock::time_point start;
    std::ofstream outFS;

signals:
    /**
     * Signal used as soon as the extraction from concept net is finished
     */
    void jsonExtracted();
};
} // namespace kbcr
