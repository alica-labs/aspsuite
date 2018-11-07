#include "gui/ModelSettingDialog.h"
#include <gui/KnowledgebaseCreator.h>

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetConcept.h"
#include "containers/ConceptNetEdge.h"

#include <sstream>

namespace kbcr
{

ConceptNetCall::ConceptNetCall(KnowledgebaseCreator* gui, QString id, QString queryConcept)
{
    this->gui = gui;
    this->id = id;
    this->nextEdgesPage = nextEdgesPage;
    this->collectSynonymConceptsNAM = new QNetworkAccessManager(this);
    this->collectAntonymConceptsNAM = new QNetworkAccessManager(this);
    this->collectUsedForConceptsNAM = new QNetworkAccessManager(this);
    this->queryConcept = queryConcept;
    this->connect(this->collectSynonymConceptsNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(collectConcepts(QNetworkReply*)));
    this->connect(this->collectAntonymConceptsNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(mapAntonyms(QNetworkReply*)));
    this->connect(this->collectUsedForConceptsNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(mapServices(QNetworkReply*)));
    this->newConceptFound = false;
}

ConceptNetCall::~ConceptNetCall()
{
    //        delete this->collectAntonymConceptsNAM;
    //        delete this->collectSynonymConceptsNAM;
}

std::string ConceptNetCall::toString()
{
    std::stringstream ss;
    ss << "ConceptNetCall:" << std::endl;
    ss << "ID: " << this->id.toStdString() << std::endl;
    ss << "Edges:" << std::endl;
    for (auto edge : this->edges) {
        ss << edge->toString();
    }
    ss << "NextEdgePage: " << this->nextEdgesPage.toStdString() << std::endl;
    ss << "Concepts: " << std::endl;
    for (auto concept : this->concepts) {
        ss << concept.toStdString() << " ";
    }
    ss << "Adjectives: " << std::endl;
    for (auto adjective : this->connectedConcepts) {
        ss << adjective.first.toStdString() << " ";
    }
    ss << std::endl;
    return ss.str();
}

void ConceptNetCall::findServices()
{
    if (this->queryConcept.compare("wildcard") == 0) {
        return;
    }

    std::cout << "ConceptNetCall: queryConcept " << this->queryConcept.toStdString() << std::endl;
    for (auto edge : this->edges) {
        if (edge->relation.compare("MotivatedByGoal") == 0) {
            if (edge->secondConcept->term.compare(this->queryConcept) == 0) {
                this->connectedConcepts.emplace(edge->firstConcept->term, edge);
                std::cout << "ConceptNetCall: edge " << edge->firstConcept->term.toStdString() << std::endl;
            }
        }
    }

    collectMotivatedByGoals();
    for (auto pair : this->motivatedConceptMap) {
        std::cout << "MotivatedConcept: " << pair.first.toStdString() << std::endl;
        std::cout << "\t UsedFor: ";
        for (auto usedFor : pair.second) {
            std::cout << usedFor.toStdString() << " ";
        }
        std::cout << std::endl;
    }
}

void ConceptNetCall::collectMotivatedByGoals()
{
    for (auto motivatedConcepts : this->connectedConcepts) {
        this->motivatedConceptMap.emplace(motivatedConcepts.first, std::vector<QString>());
        this->currentMotivatedConcept = motivatedConcepts.first;
        QUrl url(KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYEND + motivatedConcepts.first + "&rel=/r/UsedFor" +
                 "&limit=1000");
        QEventLoop loop;
        this->connect(this, SIGNAL(closeCollectLoop()), &loop, SLOT(quit()));
        this->callUrl(url, this->collectUsedForConceptsNAM);
        loop.exec();
    }
    this->collectUsedForConceptsNAM->deleteLater();
}

void ConceptNetCall::mapServices(QNetworkReply* reply)
{
    // not current adjective and not already in
    QString data = reply->readAll();
    // remove html part
    std::string fullData = data.toStdString();
    auto start = fullData.find("{\"@context\":");
    auto end = fullData.find("</script>");
    fullData = fullData.substr(start, end - start);
    // parse json
    auto jsonString = QString(fullData.c_str()).toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
    QJsonArray edges = jsonDoc.object()["edges"].toArray();
    for (int i = 0; i < std::min(edges.size(), 5); i++) { // best x edges only
        QJsonObject edge = edges[i].toObject();
        double weight = edge["weight"].toDouble();
        if (weight < this->gui->modelSettingsDialog->getMinCn5Weight()) {
            break;
        }
        // end of edge
        QJsonObject end = edge["end"].toObject();
        QString endLanguage = end["language"].toString();
        // skip non English
        if (endLanguage != "en") {
            continue;
        }
        QString endTerm = end["term"].toString();
        endTerm = trimTerm(endTerm);
        if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm)) {
            //				std::cout << "ConceptNetCall: Skipping Antonym:" << endTerm.toStdString() << std::endl;
            continue;
        }
        // start of edge
        QJsonObject start = edge["start"].toObject();
        QString startLanguage = start["language"].toString();
        // skip non English
        if (startLanguage != "en") {
            continue;
        }
        QString startTerm = start["term"].toString();
        startTerm = trimTerm(startTerm);
        if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm)) {
            //				std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
            continue;
        }

        auto tmp = this->motivatedConceptMap.at(currentMotivatedConcept);
        if (std::find(tmp.begin(), tmp.end(), startTerm) == tmp.end()) {
            this->motivatedConceptMap.at(currentMotivatedConcept).push_back(startTerm);
        }
    }
    if (this->motivatedConceptMap.at(currentMotivatedConcept).size() == 0) {
        this->motivatedConceptMap.erase(currentMotivatedConcept);
    }
    emit closeCollectLoop();
}

void ConceptNetCall::findInconsistencies()
{
    if (this->queryConcept.compare("wildcard") == 0) {
        return;
    }
    for (auto edge : this->edges) {
        if (edge->firstConcept->senseLabel.compare("a") == 0 && edge->firstConcept->term.compare(this->queryConcept) != 0) {
            if (this->connectedConcepts.find(edge->firstConcept->term) == this->connectedConcepts.end()) {
                this->connectedConcepts.emplace(edge->firstConcept->term, edge);
            }
        }
        if (edge->secondConcept->senseLabel.compare("a") == 0 && edge->secondConcept->term.compare(this->queryConcept) != 0) {
            if (this->connectedConcepts.find(edge->secondConcept->term) == this->connectedConcepts.end()) {
                this->connectedConcepts.emplace(edge->secondConcept->term, edge);
            }
        }
    }
    //		std::cout << "ConceptNetCall: Adjectives size: " << this->connectedConcepts.size() << std::endl;
    collectAntonyms();
    //		std::cout << "ConceptNetCall: Finished collecting Antonyms." << std::endl;
    checkAdjectives();
    //		std::cout << "ConceptNetCall: Finished checking Antonyms among the connectedConcepts of the queried concept."
    //				<< std::endl;
    for (auto adj : this->connectedConcepts) {
        auto tmp = std::make_shared<std::vector<QString>>();
        tmp->push_back(adj.first);
        this->gatherMap.emplace(adj.first, tmp);
    }
    gatherConcepts(this->connectedConcepts);
    //		std::cout << "ConceptNetCall: Finished gathering related concepts. Number of found concepts: "
    //				<< gatheredConcepts.size() << std::endl;
    for (auto adj : this->connectedConcepts) {
        if (this->gatherMap.at(adj.first)->size() == 1) {
            this->gatherMap.erase(adj.first);
        }
    }
    checkGatheredConcepts();
    //		std::cout << "ConceptNetCall: Finished checking Antonyms among gathered concepts." << std::endl;
}

void ConceptNetCall::checkGatheredConcepts()
{
    //		for (auto i : this->gatherMap)
    //		{
    //			std::cout << i.first.toStdString() << ": ";
    //			for (auto j : *i.second)
    //			{
    //				std::cout << j.toStdString() << " ";
    //			}
    //			std::cout << std::endl;
    //		}
    std::map<QString, std::vector<QString>>::iterator it;
    bool conceptDeleted = false;
    for (it = this->adjectiveAntonymMap.begin(); it != this->adjectiveAntonymMap.end();) {
        if (this->adjectiveAntonymMap.find(it->first) == this->adjectiveAntonymMap.end()) {
            it++;
            continue;
        }
        for (auto gathered : this->gatherMap) {
            for (auto concept : *gathered.second) {
                if (std::find(it->second.begin(), it->second.end(), concept) != it->second.end()) {
                    if (this->connectedConcepts.find(gathered.first) == this->connectedConcepts.end()) {
                        continue;
                    }
                    if (this->connectedConcepts.at(it->first)->weight < this->connectedConcepts.at(gathered.first)->weight) {
                        auto iterator = this->connectedConcepts.find(it->first);
                        if (iterator != this->connectedConcepts.end()) {
                            //								std::cout << "ConceptNetCall: Antonym found: removing: " <<
                            //it->first.toStdString()
                            //										<< " keeping: " << gathered.first.toStdString() <<
                            //std::endl;
                            this->connectedConcepts.erase(it->first);
                            this->adjectiveAntonymMap.erase(it->first);
                            conceptDeleted = true;
                        }
                    } else {
                        auto iterator = this->connectedConcepts.find(gathered.first);
                        if (iterator != this->connectedConcepts.end()) {
                            //								std::cout << "ConceptNetCall: Antonym found: removing: " <<
                            //gathered.first.toStdString()
                            //										<< " keeping: " << it->first.toStdString() << std::endl;
                            this->connectedConcepts.erase(gathered.first);
                            this->adjectiveAntonymMap.erase(gathered.first);
                            conceptDeleted = true;
                        }
                    }
                }
            }
        }
        if (!conceptDeleted) {
            it++;
        } else {
            conceptDeleted = false;
        }
    }
}

void ConceptNetCall::checkAdjectives(/*std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck*/)
{
    std::map<QString, std::vector<QString>>::iterator it;
    bool conceptDeleted = false;
    for (it = this->adjectiveAntonymMap.begin(); it != this->adjectiveAntonymMap.end();) {
        if (this->adjectiveAntonymMap.find(it->first) == this->adjectiveAntonymMap.end()) {
            it++;
            continue;
        }
        for (auto antonym : it->second) {
            if (this->connectedConcepts.find(antonym) == this->connectedConcepts.end()) {
                continue;
            }
            if (this->connectedConcepts.at(it->first)->weight < this->connectedConcepts.at(antonym)->weight) {
                auto iterator = this->connectedConcepts.find(it->first);
                if (iterator != this->connectedConcepts.end()) {
                    //						std::cout << "ConceptNetCall: Antonym found: removing: " << it->first.toStdString()
                    //								<< " keeping: " << antonym.toStdString() << std::endl;
                    this->connectedConcepts.erase(it->first);
                    this->adjectiveAntonymMap.erase(it->first);
                    conceptDeleted = true;
                }
            } else {
                auto iterator = this->connectedConcepts.find(antonym);
                if (iterator != this->connectedConcepts.end()) {
                    //						std::cout << "ConceptNetCall: Antonym found: removing: " << antonym.toStdString()
                    //								<< " keeping: " << it->first.toStdString() << std::endl;
                    this->connectedConcepts.erase(antonym);
                    this->adjectiveAntonymMap.erase(antonym);
                    conceptDeleted = true;
                }
            }
        }
        if (!conceptDeleted) {
            it++;
        } else {
            conceptDeleted = false;
        }
    }
}

QString ConceptNetCall::trimTerm(QString term)
{
    auto pos = term.lastIndexOf("/");
    return term.right(term.length() - pos - 1);
}

void ConceptNetCall::gatherConcepts(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck)
{
    this->newConceptFound = false;
    for (auto adjective : toCheck) {
        this->currentConcept = adjective.first;
        QEventLoop loopIsA;
        this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopIsA, SLOT(quit()));
        QUrl urlIsA(
                KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYSTART + adjective.first + "&rel=/r/IsA" + "&limit=1000");
        this->callUrl(urlIsA, this->collectSynonymConceptsNAM);
        loopIsA.exec();
        QEventLoop loopSynonym;
        this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopSynonym, SLOT(quit()));
        QUrl urlSynonym(KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYSTART + adjective.first + "&rel=/r/Synonym" +
                        "&limit=1000");
        this->callUrl(urlSynonym, this->collectSynonymConceptsNAM);
        loopSynonym.exec();
        QEventLoop loopDefinedAs;
        this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopDefinedAs, SLOT(quit()));
        QUrl urlDefinedAs(KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYSTART + adjective.first + "&rel=/r/DefinedAs" +
                          "&limit=1000");
        this->callUrl(urlDefinedAs, this->collectSynonymConceptsNAM);
        loopDefinedAs.exec();
        QEventLoop loopPartOf;
        this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopPartOf, SLOT(quit()));
        QUrl urlPartOf(KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYSTART + adjective.first + "&rel=/r/PartOf" +
                       "&limit=1000");
        this->callUrl(urlPartOf, this->collectSynonymConceptsNAM);
        loopPartOf.exec();
        this->conceptsToCheck.erase(adjective.first);
    }
    if (this->newConceptFound) {
        gatherConcepts(this->conceptsToCheck);
    } else {
        this->collectSynonymConceptsNAM->deleteLater();
    }
}

void ConceptNetCall::collectConcepts(QNetworkReply* reply)
{
    QString data = reply->readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data.toUtf8()));
    QJsonArray edges = jsonDoc.object()["edges"].toArray();
    for (int i = 0; i < edges.size(); i++) {
        QJsonObject edge = edges[i].toObject();
        auto cn5Edge = extractCNEdge(edge);
        if (cn5Edge == nullptr) {
            continue;
        }
        if (cn5Edge->id.compare("BREAK") == 0) {
            break;
        }
        QString endTerm = edge["end"].toObject()["term"].toString();
        endTerm = trimTerm(endTerm);
        if (this->conceptsToCheck.find(endTerm) == this->conceptsToCheck.end() && this->gatheredConcepts.find(endTerm) == this->gatheredConcepts.end() &&
                cn5Edge->secondConcept->senseLabel.compare("a") == 0) {
            this->conceptsToCheck.emplace(endTerm, cn5Edge);
            this->gatheredConcepts.emplace(endTerm, std::pair<QString, std::shared_ptr<ConceptNetEdge>>(this->currentConcept, cn5Edge));
            this->newConceptFound = true;
            for (auto p : this->gatherMap) {
                if (std::find(p.second->begin(), p.second->end(), this->currentConcept) != p.second->end()) {
                    p.second->push_back(endTerm);
                    break;
                }
            }
        }
    }
    emit closeLoopAdjectiveGathering();
}

bool ConceptNetCall::conceptContainsUTF8(QString concept)
{
    for (int i = 0; i < concept.length(); i++) {
        if (concept.at(i).unicode() > 127) {
            return true;
        }
    }
    return false;
}

void ConceptNetCall::callUrl(QUrl url, QNetworkAccessManager* n)
{
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    n->get(request);
}

std::shared_ptr<ConceptNetEdge> ConceptNetCall::extractCNEdge(QJsonObject edge)
{
    QString edgeId = edge["@id"].toString();
    double weight = edge["weight"].toDouble();
    if (weight < this->gui->modelSettingsDialog->getMinCn5Weight()) {
        return std::make_shared<ConceptNetEdge>(QString("BREAK"), QString(""), nullptr, nullptr, QString(""), -1.0);
    }
    // end of edge
    QJsonObject end = edge["end"].toObject();
    QString endLanguage = end["language"].toString();
    // skip non English
    if (endLanguage != "en") {
        return nullptr;
    }
    QString endTerm = end["term"].toString();
    endTerm = trimTerm(endTerm);
    if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm)) {
        //			std::cout << "ConceptNetCall: Skipping concept:" << endTerm.toStdString() << std::endl;
        return nullptr;
    }
    QString endSenseLabel = end["sense_label"].toString();
    QString endID = end["@id"].toString();
    // start of edge
    QJsonObject start = edge["start"].toObject();
    QString startLanguage = start["language"].toString();
    // skip non English
    if (startLanguage != "en") {
        return nullptr;
    }
    QString startTerm = start["term"].toString();
    startTerm = trimTerm(startTerm);
    if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm)) {
        //			std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
        return nullptr;
    }
    QString startSenseLabel = start["sense_label"].toString();
    QString startID = start["@id"].toString();
    QString relation = edge["rel"].toObject()["@id"].toString();
    relation = relation.right(relation.size() - relation.lastIndexOf('/') - 1);
    return std::make_shared<ConceptNetEdge>(edgeId, startLanguage, std::make_shared<ConceptNetConcept>(startTerm, startSenseLabel, startID),
            std::make_shared<ConceptNetConcept>(endTerm, endSenseLabel, endID), relation, weight);
}

void ConceptNetCall::mapAntonyms(QNetworkReply* reply)
{
    // not current adjective and not already in
    QString data = reply->readAll();
    // remove html part
    std::string fullData = data.toStdString();
    auto start = fullData.find("{\"@context\":");
    auto end = fullData.find("</script>");
    fullData = fullData.substr(start, end - start);
    // parse json
    auto jsonString = QString(fullData.c_str()).toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
    QJsonArray edges = jsonDoc.object()["edges"].toArray();
    for (int i = 0; i < edges.size(); i++) {
        QJsonObject edge = edges[i].toObject();
        double weight = edge["weight"].toDouble();
        if (weight < this->gui->modelSettingsDialog->getMinCn5Weight()) {
            break;
        }
        // end of edge
        QJsonObject end = edge["end"].toObject();
        QString endLanguage = end["language"].toString();
        // skip non English
        if (endLanguage != "en") {
            continue;
        }
        QString endTerm = end["term"].toString();
        endTerm = trimTerm(endTerm);
        if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm)) {
            //				std::cout << "ConceptNetCall: Skipping Antonym:" << endTerm.toStdString() << std::endl;
            continue;
        }
        // start of edge
        QJsonObject start = edge["start"].toObject();
        QString startLanguage = start["language"].toString();
        // skip non English
        if (startLanguage != "en") {
            continue;
        }
        QString startTerm = start["term"].toString();
        startTerm = trimTerm(startTerm);
        if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm)) {
            //				std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
            continue;
        }
        if (endTerm != currentAdjective) {
            auto tmp = this->adjectiveAntonymMap.at(currentAdjective);
            if (std::find(tmp.begin(), tmp.end(), endTerm) == tmp.end()) {
                this->adjectiveAntonymMap.at(currentAdjective).push_back(endTerm);
            }
        }
        if (startTerm != currentAdjective) {
            auto tmp = this->adjectiveAntonymMap.at(currentAdjective);
            if (std::find(tmp.begin(), tmp.end(), startTerm) == tmp.end()) {
                this->adjectiveAntonymMap.at(currentAdjective).push_back(startTerm);
            }
        }
    }
    if (this->adjectiveAntonymMap.at(currentAdjective).size() == 0) {
        this->adjectiveAntonymMap.erase(currentAdjective);
    }
    emit closeCollectLoop();
}

void ConceptNetCall::collectAntonyms()
{
    for (auto adjective : this->connectedConcepts) {
        this->adjectiveAntonymMap.emplace(adjective.first, std::vector<QString>());
        this->currentAdjective = adjective.first;
        QUrl url(KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYNODE + adjective.first + "&rel=/r/Antonym" +
                 "&limit=1000");
        QEventLoop loop;
        this->connect(this, SIGNAL(closeCollectLoop()), &loop, SLOT(quit()));
        this->callUrl(url, this->collectAntonymConceptsNAM);
        loop.exec();
    }
    this->collectAntonymConceptsNAM->deleteLater();
    //		for (auto pair : this->adjectiveAntonymMap)
    //		{
    //			std::cout << "Adjective: " << pair.first.toStdString() << std::endl;
    //			std::cout << "\t Antonyms: ";
    //			for (auto antonym : pair.second)
    //			{
    //				std::cout << antonym.toStdString() << " ";
    //			}
    //			std::cout << std::endl;
    //		}
}

} /* namespace kbcr */
