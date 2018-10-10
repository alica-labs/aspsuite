#include "commands/OfferServicesCommand.h"

#include "commands/GroundCommand.h"
#include "commands/SolveCommand.h"
#include "commands/LoadSavedProgramCommand.h"

#include "containers/ConceptNetConcept.h"
#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"

#include "handler/CommandHistoryHandler.h"
#include "handler/SaveLoadHandler.h"

#include "gui/ModelSettingDialog.h"
#include "gui/KnowledgebaseCreator.h"

#include "ui_knowledgebasecreator.h"

#include <asp_solver/ASPSolver.h>

namespace kbcr {

    OfferServicesCommand::OfferServicesCommand(KnowledgebaseCreator *gui, QString queryString) {
        this->type = "Offer Services";
        this->prefixLength = KnowledgebaseCreator::CONCEPTNET_PREFIX.size();
        this->gui = gui;
        this->queryString = queryString.trimmed();
        if (this->queryString[this->queryString.size() - 1] == '.') {
            this->queryString = this->queryString.remove(this->queryString.size() - 1, 1);
        }
        this->currentConceptNetCall = nullptr;
        this->nam = new QNetworkAccessManager(this);
        // connect signals form handling the http requests
        this->connect(this->nam, SIGNAL(finished(QNetworkReply * )), this,
                      SLOT(conceptNetCallFinished(QNetworkReply * )));
        this->connect(this, SIGNAL(jsonExtracted()), this, SLOT(extractASPPredicates()));
        this->minWeightPassed = false;
    }

    OfferServicesCommand::~OfferServicesCommand() {
    }

    void OfferServicesCommand::execute() {
        if (!this->queryString.startsWith(KnowledgebaseCreator::CONCEPTNET_PREFIX)) {
            QMessageBox msgBox;
            msgBox.setText(QString("ConceptNet 5 queries have to start with the prefix \"" +
                                   KnowledgebaseCreator::CONCEPTNET_PREFIX + "\"."));
            msgBox.setWindowModality(Qt::NonModal);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            // show message box
            int ret = msgBox.exec();
            this->undo();
            return;
        }
        if (this->queryString.contains("(") || this->queryString.contains(")")) {
            //wrong input relation
            handleWrongInput();
        } else {
            this->queryString = queryString.mid(this->prefixLength, queryString.length() - this->prefixLength);
            QUrl url(
                    KnowledgebaseCreator::CONCEPTNET_BASE_URL + KnowledgebaseCreator::CONCEPTNET_URL_QUERYNODE
                    + this->queryString
                    + "&limit=1000");
            callUrl(url);
        }
        this->gui->chHandler->addToCommandHistory(shared_from_this());
        this->gui->enableGui(false);
    }

    void OfferServicesCommand::undo() {
        this->gui->chHandler->removeFromCommandHistory(shared_from_this());
    }

    QJsonObject OfferServicesCommand::toJSON() {
        QJsonObject ret;
        ret["type"] = "Offer Services";
        ret["Service"] = this->queryString;
        return ret;
    }

    void OfferServicesCommand::handleWrongInput() {
        this->undo();
    }

    void OfferServicesCommand::callUrl(QUrl url) {
        QNetworkRequest request(url);
        request.setRawHeader("Accept", "application/json");
        this->nam->get(request);
    }

    void OfferServicesCommand::conceptNetCallFinished(QNetworkReply *reply) {
        //get data
        QString data = reply->readAll();
        //remove html part
        QJsonDocument jsonDoc(QJsonDocument::fromJson(data.toUtf8()));
        QString id = jsonDoc.object()["@id"].toString();
        // save next page form following get request
        QString nextPage = jsonDoc.object()["view"].toObject()["nextPage"].toString();
#ifdef ConceptNetQueryCommandDebug
        cout << id.toStdString() << endl;
        cout << nextPage.toStdString() << endl;
#endif
        // only null in first query
        if (this->currentConceptNetCall == nullptr) {
            this->currentConceptNetCall = new ConceptNetCall(this->gui, id, this->queryString);
        }
        this->currentConceptNetCall->nextEdgesPage = nextPage;
        // extract edges
        QJsonArray edges = jsonDoc.object()["edges"].toArray();
        for (int i = 0; i < edges.size(); i++) {
            QJsonObject edge = edges[i].toObject();
            double weight = edge["weight"].toDouble();
            if (weight < this->gui->modelSettingsDialog->getMinCn5Weight()) {
                this->minWeightPassed = true;
                break;
            }
            //end of edge
            QString edgeId = edge["@id"].toString();
            QJsonObject end = edge["end"].toObject();
            QString endLanguage = end["language"].toString();
            // skip non English
            if (endLanguage != "en") {
                continue;
            }
            QString endTerm = end["term"].toString();
            endTerm = trimTerm(endTerm);
            if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm)) {
                std::cout << "ConceptNetQueryCommand: Skipping Concept:" << endTerm.toStdString() << std::endl;
                continue;
            }
            QString endSenseLabel = end["sense_label"].toString();
            QString endID = end["@id"].toString();
            if (find(this->currentConceptNetCall->concepts.begin(), this->currentConceptNetCall->concepts.end(),
                     endTerm) == this->currentConceptNetCall->concepts.end()) {
                this->currentConceptNetCall->concepts.push_back(endTerm);
            }
            //start of edge
            QJsonObject start = edge["start"].toObject();
            QString startLanguage = start["language"].toString();
            // skip non English
            if (startLanguage != "en") {
                continue;
            }
            QString startTerm = start["term"].toString();
            startTerm = trimTerm(startTerm);
            if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm)) {
                std::cout << "ConceptNetQueryCommand: Skipping concept:" << startTerm.toStdString() << std::endl;
                continue;
            }
            QString startSenseLabel = start["sense_label"].toString();
            QString startID = start["@id"].toString();
            if (find(this->currentConceptNetCall->concepts.begin(), this->currentConceptNetCall->concepts.end(),
                     startTerm) == this->currentConceptNetCall->concepts.end()) {
                this->currentConceptNetCall->concepts.push_back(startTerm);
            }
            QString relation = edge["rel"].toObject()["@id"].toString();
            relation = relation.right(relation.size() - relation.lastIndexOf('/') - 1);
            // sources
            QJsonArray sources = edge["sources"].toArray();
            auto tmp = std::make_shared<ConceptNetEdge>(
                    edgeId, startLanguage, make_shared<ConceptNetConcept>(startTerm, startSenseLabel, startID),
                    make_shared<ConceptNetConcept>(endTerm, endSenseLabel, endID), relation, weight);
            for (int j = 0; j < sources.size(); j++) {
                tmp->sources.push_back(sources[j].toObject()["contributor"].toString());
            }
            this->currentConceptNetCall->edges.push_back(tmp);
        }
        if (!nextPage.isEmpty() && !this->minWeightPassed) {
//			std::cout << nextPage.toStdString() << std::endl;
            QUrl url(KnowledgebaseCreator::CONCEPTNET_BASE_URL + nextPage);
            callUrl(url);
        } else {
#ifdef ConceptNetQueryCommandDebug
            cout << this->currentConceptNetCall->toString();
#endif
            std::cout << "Number of connected Concepts: " << this->currentConceptNetCall->edges.size() << std::endl;

            this->currentConceptNetCall->findServices();
            emit jsonExtracted();
        }

    }

    void OfferServicesCommand::extractASPPredicates() {
        QString programSection = "#program cn5_commonsenseKnowledge";
        QString program = programSection;
        program.append(".\n");
        auto tmp = createASPPredicates();
        program.append(tmp);
        std::shared_ptr<GroundCommand> gc = std::make_shared<GroundCommand>(this->gui, program);
        gc->execute();
        this->gui->chHandler->removeFromCommandHistory(gc);
        std::shared_ptr<SolveCommand> sc = std::make_shared<SolveCommand>(this->gui);
        sc->execute();
        this->gui->chHandler->removeFromCommandHistory(sc);
        auto pgmMap = extractBackgroundKnowledgePrograms(tmp);
        for (auto pair : pgmMap) {
            this->gui->getSolver()->add(programSection.toStdString().c_str(), {}, pair.second.toStdString().c_str());
            this->gui->getUi()->programLabel->setText(
                    this->gui->getUi()->programLabel->text().append("\n").append(pair.second).append("\n"));
        }
        this->gui->enableGui(true);
        if (this->gui->slHandler->currentLoadCmd != nullptr) {
            emit
            this->gui->slHandler->currentLoadCmd->cn5CallFinished();
        }
        this->gui->getUi()->conceptNetBtn->setFocus();
    }

    QString OfferServicesCommand::conceptToASPPredicate(QString concept) {
        if (concept.contains('.')) {
            concept.replace('.', '_');
        }
        if (concept.contains(',')) {
            concept.replace(',', '_');
        }
        if (concept.contains(' ')) {
            concept.replace(' ', '_');
        }
        return concept;
    }

    QString OfferServicesCommand::createASPPredicates() {
        QString ret = "";
        for (auto edge : this->currentConceptNetCall->edges) {
            if (edge->weight < this->gui->modelSettingsDialog->getMinCn5Weight()) {
                continue;
            }
            QString tmp = "";
            tmp.append(KnowledgebaseCreator::CONCEPTNET_PREFIX).append(edge->relation);
            tmp.append("(").append(KnowledgebaseCreator::CONCEPTNET_PREFIX).append(
                    conceptToASPPredicate(edge->firstConcept->term)).append(
                    ", ").append(
                    KnowledgebaseCreator::CONCEPTNET_PREFIX).append(
                    conceptToASPPredicate(edge->secondConcept->term).append(", ").append(
                            QString::number((int) (edge->weight * edge->sources.size())))).append(").\n");
            ret.append(tmp);
        }
        return ret;
    }

    std::map<QString, QString> OfferServicesCommand::extractBackgroundKnowledgePrograms(QString conceptNetProgram) {
        std::map<QString, QString> ret;
        std::vector<QString> addedRelations;
        for (auto edge : this->currentConceptNetCall->edges) {
            QString tmpRel = edge->relation;
            tmpRel[0] = tmpRel[0].toLower();
            auto it = find(addedRelations.begin(), addedRelations.end(), tmpRel);
            if (it == addedRelations.end()) {
                addedRelations.push_back(tmpRel);
                QString pgm = "#program cn5_situationalKnowledge(n,m).\n";
                pgm.append("#external -").append(tmpRel).append("(n, m).\n");
                pgm.append(createBackgroundKnowledgeRule(tmpRel, edge));
                ret.emplace(tmpRel, pgm);
            } else {
                ret.at(tmpRel).append(createBackgroundKnowledgeRule(tmpRel, edge));
            }
        }
#ifdef ConceptNetQueryCommandDebug
        for(auto pgm : ret)
        {
            std::cout << pgm.second.toStdString() << std::endl;
        }
#endif
        return ret;
    }

    QString OfferServicesCommand::createBackgroundKnowledgeRule(QString relation, std::shared_ptr<ConceptNetEdge> edge) {
        QString ret = relation;
        ret.append("(n, m, W) :- not -").append(relation).append("(n, m), typeOf(n, ").append(
                KnowledgebaseCreator::CONCEPTNET_PREFIX).append(
                conceptToASPPredicate(edge->firstConcept->term)).append("), typeOf(m, ").append(
                KnowledgebaseCreator::CONCEPTNET_PREFIX).append(
                conceptToASPPredicate(edge->secondConcept->term)).append("), ").append(
                KnowledgebaseCreator::CONCEPTNET_PREFIX).append(
                relation.replace(0, 1, relation.at(0).toUpper())).append("(n, m, W).\n");
        return ret;
    }

    bool OfferServicesCommand::conceptContainsUTF8(QString concept) {
        for (int i = 0; i < concept.length(); i++) {
            if (concept.at(i).unicode() > 127) {
                return true;
            }
        }
        return false;
    }

    QString OfferServicesCommand::trimTerm(QString term) {
        auto pos = term.lastIndexOf("/");
        return term.right(term.length() - pos - 1);
    }


} /* namespace kbcr */
