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
//		this->nam2 = new QNetworkAccessManager(this);
		this->checkNAM = new QNetworkAccessManager(this);
		this->currentAdjectiveIndex = -1;
		this->queryConcept = queryConcept;
		this->connect(this->nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(removeIfAntonym(QNetworkReply*)));
//		this->connect(this->nam2, SIGNAL(finished(QNetworkReply*)), this, SLOT(mapAntonyms(QNetworkReply*)));
		this->connect(this->checkNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(collectConcepts(QNetworkReply*)));
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
		//TODO perhaps needed later otherwise remove
//		collectAntonyms();
		checkFirstAdjectives();
		std::cout << "ConceptNetCall: finished checking Antonyms among the adjectives of the queried concept."
				<< std::endl;
		inconsistencySearch();
	}

	void ConceptNetCall::checkFirstAdjectives()
	{
		auto tmp = this->adjectives;
		for (auto firstAdjective : tmp)
		{
			for (auto secondAdjective : tmp)
			{
				if (firstAdjective == secondAdjective)
				{
					continue;
				}
				this->currentAntonymCheck = std::pair<QString, QString>(firstAdjective.first, secondAdjective.first);
				QUrl url(
						"http://api.localhost/query?node=/c/en/" + firstAdjective.first + "&other=/c/en/"
								+ secondAdjective.first + "&rel=/r/Antonym");
				this->callUrl(url, this->nam);
				QEventLoop loop;
				this->connect(this, SIGNAL(closeLoopFirstAdjectives()), &loop, SLOT(quit()));
				loop.exec();
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
			if (this->adjectives.find(this->currentAntonymCheck.first) == this->adjectives.end()
					|| this->adjectives.find(this->currentAntonymCheck.second) == this->adjectives.end())
			{
				emit closeLoopFirstAdjectives();
				return;
			}
			std::cout << "First weight: " << this->adjectives.at(this->currentAntonymCheck.first)->weight
					<< " Second weight: " << this->adjectives.at(this->currentAntonymCheck.second)->weight << std::endl;
			std::cout << "ConceptNetCall: Antonym found: ";
			if (this->adjectives.at(this->currentAntonymCheck.first)
					< this->adjectives.at(this->currentAntonymCheck.second))
			{
				std::cout << ": removing " << this->currentAntonymCheck.first.toStdString() << " : keeping: "
						<< this->currentAntonymCheck.second.toStdString() << std::endl;
				this->adjectives.erase(this->currentAntonymCheck.first);
			}
			else
			{
				std::cout << ": removing " << this->currentAntonymCheck.second.toStdString() << " : keeping: "
						<< this->currentAntonymCheck.first.toStdString() << std::endl;
				this->adjectives.erase(this->currentAntonymCheck.second);
			}
		}
		emit closeLoopFirstAdjectives();
	}

//	void ConceptNetCall::mapAntonyms(QNetworkReply* reply)
//	{
//		//not current adjective and not already in
//		QString data = reply->readAll();
//		//remove html part
//		std::string fullData = data.toStdString();
//		auto start = fullData.find("{\"@context\":");
//		auto end = fullData.find("</script>");
//		fullData = fullData.substr(start, end - start);
//		// parse json
//		auto jsonString = QString(fullData.c_str()).toUtf8();
//		QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
//		QJsonArray edges = jsonDoc.object()["edges"].toArray();
//		for (int i = 0; i < edges.size(); i++)
//		{
//			QJsonObject edge = edges[i].toObject();
//			double weight = edge["weight"].toDouble();
//			if (weight < this->gui->modelSettingsDialog->getMinCn5Weight())
//			{
//				continue;
//			}
//			//end of edge
//			QJsonObject end = edge["end"].toObject();
//			QString endLanguage = end["language"].toString();
//			// skip non English
//			if (endLanguage != "en")
//			{
//				continue;
//			}
//			QString endTerm = end["term"].toString();
//			endTerm = trimTerm(endTerm);
//			if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm))
//			{
//				std::cout << "ConceptNetCall: Skipping Antonym:" << endTerm.toStdString() << std::endl;
//				continue;
//			}
//			//start of edge
//			QJsonObject start = edge["start"].toObject();
//			QString startLanguage = start["language"].toString();
//			// skip non English
//			if (startLanguage != "en")
//			{
//				continue;
//			}
//			QString startTerm = start["term"].toString();
//			startTerm = trimTerm(startTerm);
//			if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm))
//			{
//				std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
//				continue;
//			}
//			if (endTerm != this->concepts.at(currentAdjectiveIndex))
//			{
//				auto tmp = this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex));
//				if (find(tmp.begin(), tmp.end(), endTerm) == tmp.end())
//				{
//					this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex)).push_back(endTerm);
//				}
//			}
//			if (startTerm != this->concepts.at(currentAdjectiveIndex))
//			{
//				auto tmp = this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex));
//				if (find(tmp.begin(), tmp.end(), startTerm) == tmp.end())
//				{
//					this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex)).push_back(startTerm);
//				}
//			}
//		}
//		emit closeLoop2();
//	}

//	void ConceptNetCall::collectAntonyms()
//	{
//		for (auto concept : this->concepts)
//		{
//			this->adjectiveAntonymMap.emplace(concept, std::vector<QString>());
//			this->currentAdjectiveIndex++;
//			QUrl url("http://api.localhost/query?node=/c/en/" + concept + "&rel=/r/Antonym");
//			QNetworkRequest request(url);
//			this->nam2->get(request);
//			QEventLoop loop;
//			this->connect(this, SIGNAL(closeLoop2()), &loop, SLOT(quit()));
//			loop.exec();
//		}
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
//	}

	QString ConceptNetCall::trimTerm(QString term)
	{
		auto pos = term.lastIndexOf("/");
		return term.right(term.length() - pos - 1);
	}

	void ConceptNetCall::inconsistencySearch()
	{
		for (auto adjective : this->adjectives)
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
		}
		//TODO call method to check antonym after this
		std::cout << "concepts to check :" << std::endl;
		for (auto item : this->conceptsToCheck)
		{
			std::cout << item.first.toStdString() << " ";
		}
		std::cout << std::endl;
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
			double weight = edge["weight"].toDouble();
			if (weight < this->gui->modelSettingsDialog->getMinCn5Weight())
			{
				continue;
			}
			//end of edge
			QJsonObject end = edge["end"].toObject();
			QString endLanguage = end["language"].toString();
			// skip non English
			if (endLanguage != "en")
			{
				continue;
			}
			QString endTerm = end["term"].toString();
			endTerm = trimTerm(endTerm);
			if (endTerm.at(0).isDigit() || this->conceptContainsUTF8(endTerm))
			{
				std::cout << "ConceptNetCall: Skipping concept:" << endTerm.toStdString() << std::endl;
				continue;
			}
			if (this->conceptsToCheck.find(endTerm) == this->conceptsToCheck.end())
			{
				this->conceptsToCheck.emplace(endTerm, weight);
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
} /* namespace cng */

