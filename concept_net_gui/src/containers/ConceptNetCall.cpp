/*
 * ConceptNetCall.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include "gui/ConceptNetGui.h"
#include "gui/ModelSettingDialog.h"

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"
#include "containers/ConceptNetConcept.h"

#include <string>
#include <sstream>
#include <iostream>

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <QEventLoop>

namespace cng
{

	ConceptNetCall::ConceptNetCall(ConceptNetGui* gui, QString id, QString queryConcept)
	{
		this->gui = gui;
		this->id = id;
		this->nextEdgesPage = nextEdgesPage;
		this->nam = new QNetworkAccessManager(this);
		this->checkNAM = new QNetworkAccessManager(this);
		this->queryConcept = queryConcept;
		this->conceptDeleted = false;
		this->connect(this->nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(removeIfAntonym(QNetworkReply*)));
		this->connect(this->checkNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(collectConcepts(QNetworkReply*)));
		this->newConceptFound = false;
	}

	ConceptNetCall::~ConceptNetCall()
	{
		delete this->nam;
//		delete this->nam2;
		delete this->checkNAM;
	}

	std::string ConceptNetCall::toString()
	{
		std::stringstream ss;
		ss << "ConceptNetCall:" << std::endl;
		ss << "ID: " << this->id.toStdString() << std::endl;
		ss << "Edges:" << std::endl;
		for (auto edge : this->edges)
		{
			ss << edge->toString();
		}
		ss << "NextEdgePage: " << this->nextEdgesPage.toStdString() << std::endl;
		ss << "Concepts: " << std::endl;
		for (auto concept : this->concepts)
		{
			ss << concept.toStdString() << " ";
		}
		ss << "Adjectives: " << std::endl;
		for (auto adjective : this->adjectives)
		{
			ss << adjective.first.toStdString() << " ";
		}
		ss << std::endl;
		return ss.str();
	}

	void ConceptNetCall::findInconsistencies()
	{
		if (this->queryConcept.compare("wildcard") == 0)
		{
			return;
		}
		for (auto edge : this->edges)
		{
			if (edge->firstConcept->senseLabel.compare("a") == 0
					&& edge->firstConcept->term.compare(this->queryConcept) != 0)
			{
				if (this->adjectives.find(edge->firstConcept->term) == this->adjectives.end())
				{
					this->adjectives.emplace(edge->firstConcept->term, edge);
				}
			}
			if (edge->secondConcept->senseLabel.compare("a") == 0
					&& edge->secondConcept->term.compare(this->queryConcept) != 0)
			{
				if (this->adjectives.find(edge->secondConcept->term) == this->adjectives.end())
				{
					this->adjectives.emplace(edge->secondConcept->term, edge);
				}
			}

		}
//		std::cout << "Adjectives before: ";
//		for (auto it : this->adjectives)
//		{
//			std::cout << it.first.toStdString() << " ";
//		}
//		std::cout << std::endl;
		checkAdjectives(this->adjectives);
		std::cout << "ConceptNetCall: finished checking Antonyms among the adjectives of the queried concept."
				<< std::endl;
		gatherConcepts(this->adjectives);
		std::cout << "ConceptNetCall: finished gathering related concepts. Number of found concepts: "
				<< checkedConcepts.size() << std::endl;
		checkAdjectives(this->checkedConcepts);
		std::cout << "ConceptNetCall: finished checking Antonyms among gathered concepts." << std::endl;
//		std::cout << "Adjectives after: ";
//		for (auto it : this->adjectives)
//		{
//			std::cout << it.first.toStdString() << " ";
//		}
//		std::cout << std::endl;
	}

	void ConceptNetCall::checkAdjectives(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck)
	{
//		auto tmp = this->adjectives;
		//was this->adjectives before test
		for (this->it = adjectives.begin(); this->it != this->adjectives.end();)
		{
			for (auto secondAdjective : toCheck)
			{
				if (*this->it == secondAdjective)
				{
					continue;
				}
				this->currentAntonymCheck = std::pair<std::pair<QString, std::shared_ptr<ConceptNetEdge>>,
						std::pair<QString, std::shared_ptr<ConceptNetEdge>>>(*this->it, secondAdjective);
				QUrl url(
						"http://api.localhost/query?node=/c/en/" + (*this->it).first + "&other=/c/en/"
								+ secondAdjective.first + "&rel=/r/Antonym");
				this->callUrl(url, this->nam);
				QEventLoop loop;
				this->connect(this, SIGNAL(closeLoopAntonym()), &loop, SLOT(quit()));
				loop.exec();
			}
			if (!this->conceptDeleted)
			{
				this->it++;
			}
			else
			{
				this->conceptDeleted = false;
			}
		}
	}

	void ConceptNetCall::removeIfAntonym(QNetworkReply* reply)
	{
		QString data = reply->readAll();
		// parse json
		QJsonDocument jsonDoc(QJsonDocument::fromJson(data.toUtf8()));
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		if (edges.size() > 0)
		{
//			if (this->adjectives.find(this->currentAntonymCheck.first) == this->adjectives.end()
//					|| this->adjectives.find(this->currentAntonymCheck.second) == this->adjectives.end())
//			{
//				emit closeLoopAntonym();
//				return;
//			}
//			std::cout << "First weight: " << this->currentAntonymCheck.first.second->toString() << "Second weight: "
//					<< this->currentAntonymCheck.second.second->toString() << std::endl;
			std::vector<std::shared_ptr<ConceptNetEdge>> inconsistency;
			inconsistency.push_back(this->currentAntonymCheck.first.second);
			inconsistency.push_back(this->currentAntonymCheck.second.second);
			inconsistency.push_back(this->extractCNEdge(edges.at(0).toObject()));
			if (this->currentAntonymCheck.first.second->weight < this->currentAntonymCheck.second.second->weight)
			{
				auto iterator = this->adjectives.find(this->currentAntonymCheck.first.first);
				if (iterator != this->adjectives.end())
				{
					this->adjectives.erase(this->it++);
					std::cout << "ConceptNetCall: Antonym found: removing: "
							<< this->currentAntonymCheck.first.first.toStdString() << " keeping: "
							<< this->currentAntonymCheck.second.first.toStdString() << std::endl;
					this->conceptDeleted = true;
				}
			}
			else
			{
				auto iterator = this->adjectives.find(this->currentAntonymCheck.second.first);
				if (iterator != this->adjectives.end())
				{
					this->adjectives.erase(this->it++);
					std::cout << "ConceptNetCall: Antonym found: removing: "
							<< this->currentAntonymCheck.second.first.toStdString() << " keeping: "
							<< this->currentAntonymCheck.first.first.toStdString() << std::endl;
					this->conceptDeleted = true;
				}
			}
		}
		emit closeLoopAntonym();
	}

	QString ConceptNetCall::trimTerm(QString term)
	{
		auto pos = term.lastIndexOf("/");
		return term.right(term.length() - pos - 1);
	}

	void ConceptNetCall::gatherConcepts(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck)
	{
		this->newConceptFound = false;
		for (auto adjective : toCheck)
		{
			QEventLoop loopIsA;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopIsA, SLOT(quit()));
			QUrl urlIsA("http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/IsA");
			this->callUrl(urlIsA, this->checkNAM);
			loopIsA.exec();
			QEventLoop loopSynonym;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopSynonym, SLOT(quit()));
			QUrl urlSynonym("http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/Synonym");
			this->callUrl(urlSynonym, this->checkNAM);
			loopSynonym.exec();
			QEventLoop loopDefinedAs;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopDefinedAs, SLOT(quit()));
			QUrl urlDefinedAs("http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/DefinedAs");
			this->callUrl(urlDefinedAs, this->checkNAM);
			loopDefinedAs.exec();
			QEventLoop loopPartOf;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopPartOf, SLOT(quit()));
			QUrl urlPartOf("http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/PartOf");
			this->callUrl(urlPartOf, this->checkNAM);
			loopPartOf.exec();
			this->checkedConcepts.emplace(adjective.first, adjective.second);
			this->conceptsToCheck.erase(adjective.first);
		}
		if (this->newConceptFound)
		{
//			std::cout << "concepts to check :" << std::endl;
//			for (auto item : this->conceptsToCheck)
//			{
//				std::cout << item.first.toStdString() << " ";
//			}
//			std::cout << std::endl;
//			std::cout << "concepts checked :" << std::endl;
//			for (auto item : this->checkedConcepts)
//			{
//				std::cout << item.first.toStdString() << " ";
//			}
//			std::cout << std::endl;
			gatherConcepts(this->conceptsToCheck);
		}
	}

	void ConceptNetCall::collectConcepts(QNetworkReply* reply)
	{
		QString data = reply->readAll();
		//remove html part
		QJsonDocument jsonDoc(QJsonDocument::fromJson(data.toUtf8()));
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		for (int i = 0; i < edges.size(); i++)
		{
			QJsonObject edge = edges[i].toObject();
			auto cn5Edge = extractCNEdge(edge);
			if (cn5Edge == nullptr)
			{
				continue;
			}
			QString endTerm = edge["end"].toObject()["term"].toString();
			endTerm = trimTerm(endTerm);
			if (this->conceptsToCheck.find(endTerm) == this->conceptsToCheck.end()
					&& this->checkedConcepts.find(endTerm) == this->checkedConcepts.end()
					&& cn5Edge->secondConcept->senseLabel.compare("a") == 0)
			{
				this->conceptsToCheck.emplace(endTerm, cn5Edge);
				this->newConceptFound = true;
			}
		}
		emit closeLoopAdjectiveGathering();
	}

	bool ConceptNetCall::conceptContainsUTF8(QString concept)
	{
		for (int i = 0; i < concept.length(); i++)
		{
			if (concept.at(i).unicode() > 127)
			{
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
		if (weight < this->gui->modelSettingsDialog->getMinCn5Weight())
		{
			return nullptr;
		}
		//end of edge
		QJsonObject end = edge["end"].toObject();
		QString endLanguage = end["language"].toString();
		// skip non English
		if (endLanguage != "en")
		{
			return nullptr;
		}
		QString endTerm = end["term"].toString();
		endTerm = trimTerm(endTerm);
		if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm))
		{
			std::cout << "ConceptNetCall: Skipping concept:" << endTerm.toStdString() << std::endl;
			return nullptr;
		}
		QString endSenseLabel = end["sense_label"].toString();
		QString endID = end["@id"].toString();
		//start of edge
		QJsonObject start = edge["start"].toObject();
		QString startLanguage = start["language"].toString();
		// skip non English
		if (startLanguage != "en")
		{
			return nullptr;
		}
		QString startTerm = start["term"].toString();
		startTerm = trimTerm(startTerm);
		if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm))
		{
			std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
			return nullptr;
		}
		QString startSenseLabel = start["sense_label"].toString();
		QString startID = start["@id"].toString();
		QString relation = edge["rel"].toObject()["label"].toString();
		return std::make_shared<ConceptNetEdge>(
				edgeId, startLanguage, std::make_shared<ConceptNetConcept>(startTerm, startSenseLabel, startID),
				std::make_shared<ConceptNetConcept>(endTerm, endSenseLabel, endID), relation, weight);
	}

} /* namespace cng */

