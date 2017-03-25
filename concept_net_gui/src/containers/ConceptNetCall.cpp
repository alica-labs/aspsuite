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
		this->nam2 = new QNetworkAccessManager(this);
		this->currentAdjectiveIndex = -1;
		this->queryConcept = queryConcept;
		this->connect(this->nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(removeIfAntonym(QNetworkReply*)));
		this->connect(this->nam2, SIGNAL(finished(QNetworkReply*)), this, SLOT(mapAntonyms(QNetworkReply*)));
//		this->connect(this, SIGNAL(nextAdjective()), this, SLOT(findAdjectives()));
	}

	ConceptNetCall::~ConceptNetCall()
	{
		delete this->nam;
		delete this->nam2;
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
			ss << adjective.toStdString() << " ";
		}
		ss << std::endl;
		return ss.str();
	}

	void ConceptNetCall::findAdjectives()
	{
		for (auto edge : this->edges)
		{
			if (edge->firstConcept->senseLabel.compare("a") == 0)
			{
				if (std::find(this->adjectives.begin(), this->adjectives.end(), edge->firstConcept->term)
						== this->adjectives.end())
				{
					this->adjectives.push_back(edge->firstConcept->term);
				}
			}
			if (edge->secondConcept->senseLabel.compare("a") == 0)
			{
				if (std::find(this->adjectives.begin(), this->adjectives.end(), edge->secondConcept->term)
						== this->adjectives.end())
				{
					this->adjectives.push_back(edge->secondConcept->term);
				}
			}

		}
		collectAntonyms();
//		checkFirstAdjectives();
	}

	void ConceptNetCall::checkFirstAdjectives()
	{
		if (this->queryConcept.compare("wildcard") == 0)
		{
			return;
		}
		//TODO check every adjective if there is an antonym edge for every other adjective remove the edge with lower weight
		for (auto firstAdjective : this->adjectives)
		{
			for (auto secondAdjective : this->adjectives)
			{
				if (firstAdjective == secondAdjective)
				{
					continue;
				}
				QUrl url(
						"http://api.localhost/query?node=/c/en/" + firstAdjective + "&other=/c/en/" + secondAdjective
								+ "&rel=/r/Antonym");
				QNetworkRequest request(url);
				this->nam->get(request);
				QEventLoop loop;
				this->connect(this, SIGNAL(closeLoop()), &loop, SLOT(quit()));
				loop.exec();
			}
		}
	}

	void ConceptNetCall::removeIfAntonym(QNetworkReply* reply)
	{
		//TODO finish
//		get data
		QString data = reply->readAll();
		//remove html part
		std::string fullData = data.toStdString();
		auto start = fullData.find("{\"@context\":");
		auto end = fullData.find("</script>");
		fullData = fullData.substr(start, end - start);
		// parse json
		auto jsonString = QString(fullData.c_str()).toUtf8();
		QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		if (edges.size() > 0)
		{
			std::cout << "Antonym found!" << std::endl;
		}
		else
		{
			std::cout << "No Antonym!" << std::endl;
		}
		emit closeLoop();
	}

	void ConceptNetCall::mapAntonyms(QNetworkReply* reply)
	{
		//not current adjective and not already in
		//		get data
		QString data = reply->readAll();
		//remove html part
		std::string fullData = data.toStdString();
		auto start = fullData.find("{\"@context\":");
		auto end = fullData.find("</script>");
		fullData = fullData.substr(start, end - start);
		// parse json
		auto jsonString = QString(fullData.c_str()).toUtf8();
		QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
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
				std::cout << "ConceptNetCall: Skipping Antonym:" << endTerm.toStdString() << std::endl;
				continue;
			}
			//start of edge
			QJsonObject start = edge["start"].toObject();
			QString startLanguage = start["language"].toString();
			// skip non English
			if (startLanguage != "en")
			{
				continue;
			}
			QString startTerm = start["term"].toString();
			startTerm = trimTerm(startTerm);
			if (startTerm.at(0).isDigit() || this->conceptContainsUTF8(startTerm))
			{
				std::cout << "ConceptNetCall: Skipping Antonym:" << startTerm.toStdString() << std::endl;
				continue;
			}
			if (endTerm != this->concepts.at(currentAdjectiveIndex))
			{
				auto tmp = this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex));
				if (find(tmp.begin(), tmp.end(), endTerm) == tmp.end())
				{
					this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex)).push_back(endTerm);
				}
			}
			if (startTerm != this->concepts.at(currentAdjectiveIndex))
			{
				auto tmp = this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex));
				if (find(tmp.begin(), tmp.end(), startTerm) == tmp.end())
				{
					this->adjectiveAntonymMap.at(this->concepts.at(currentAdjectiveIndex)).push_back(startTerm);
				}
			}
		}
		emit closeLoop2();
	}

	void ConceptNetCall::collectAntonyms()
	{
		for (auto concept : this->concepts)
		{
			this->adjectiveAntonymMap.emplace(concept, std::vector<QString>());
			this->currentAdjectiveIndex++;
			QUrl url("http://api.localhost/query?node=/c/en/" + concept + "&rel=/r/Antonym");
			QNetworkRequest request(url);
			this->nam2->get(request);
			QEventLoop loop;
			this->connect(this, SIGNAL(closeLoop2()), &loop, SLOT(quit()));
			loop.exec();
		}
		for(auto pair : this->adjectiveAntonymMap)
		{
			std::cout << "Adjective: " << pair.first.toStdString() << std::endl;
			std::cout << "\t Antonyms: ";
			for(auto antonym : pair.second)
			{
				std::cout << antonym.toStdString() << " ";
			}
			std::cout << std::endl;
		}
	}

	QString ConceptNetCall::trimTerm(QString term)
	{
		auto pos = term.lastIndexOf("/");
		return term.right(term.length() - pos - 1);
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

} /* namespace cng */

