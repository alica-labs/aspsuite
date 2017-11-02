/*
 * ConceptNetCall.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include <gui/KnowledgebaseCreator.h>
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

namespace kbcr
{

	ConceptNetCall::ConceptNetCall(KnowledgebaseCreator* gui, QString id, QString queryConcept)
	{
		this->gui = gui;
		this->id = id;
		this->nextEdgesPage = nextEdgesPage;
		this->checkNAM = new QNetworkAccessManager(this);
		this->nam2 = new QNetworkAccessManager(this);
		this->queryConcept = queryConcept;
		this->connect(this->checkNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(collectConcepts(QNetworkReply*)));
		this->connect(this->nam2, SIGNAL(finished(QNetworkReply*)), this, SLOT(mapAntonyms(QNetworkReply*)));
		this->newConceptFound = false;
	}

	ConceptNetCall::~ConceptNetCall()
	{
		delete this->nam2;
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
		std::cout << "ConceptNetCall: Adjectives size: " << this->adjectives.size() << std::endl;
		collectAntonyms();
		std::cout << "ConceptNetCall: Finished collecting Antonyms." << std::endl;
		checkAdjectives();
		std::cout << "ConceptNetCall: Finished checking Antonyms among the adjectives of the queried concept."
				<< std::endl;
		for (auto adj : this->adjectives)
		{
			auto tmp = std::make_shared<std::vector<QString>>();
			tmp->push_back(adj.first);
			this->gatherMap.emplace(adj.first, tmp);
		}
		gatherConcepts(this->adjectives);
		std::cout << "ConceptNetCall: Finished gathering related concepts. Number of found concepts: "
				<< gatheredConcepts.size() << std::endl;
		for (auto adj : this->adjectives)
		{
			if (this->gatherMap.at(adj.first)->size() == 1)
			{
				this->gatherMap.erase(adj.first);
			}
		}
		checkGatheredConcepts();
		std::cout << "ConceptNetCall: Finished checking Antonyms among gathered concepts." << std::endl;

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
		for (it = this->adjectiveAntonymMap.begin(); it != this->adjectiveAntonymMap.end();)
		{
			if (this->adjectiveAntonymMap.find(it->first) == this->adjectiveAntonymMap.end())
			{
				it++;
				continue;
			}
			for (auto gathered : this->gatherMap)
			{
				for (auto concept : *gathered.second)
				{
					if (std::find(it->second.begin(), it->second.end(), concept) != it->second.end())
					{
						if (this->adjectives.find(gathered.first) == this->adjectives.end())
						{
							continue;
						}
						if (this->adjectives.at(it->first)->weight < this->adjectives.at(gathered.first)->weight)
						{
							auto iterator = this->adjectives.find(it->first);
							if (iterator != this->adjectives.end())
							{
								std::cout << "ConceptNetCall: Antonym found: removing: " << it->first.toStdString()
										<< " keeping: " << gathered.first.toStdString() << std::endl;
								this->adjectives.erase(it->first);
								this->adjectiveAntonymMap.erase(it->first);
								conceptDeleted = true;
							}
						}
						else
						{
							auto iterator = this->adjectives.find(gathered.first);
							if (iterator != this->adjectives.end())
							{
								std::cout << "ConceptNetCall: Antonym found: removing: " << gathered.first.toStdString()
										<< " keeping: " << it->first.toStdString() << std::endl;
								this->adjectives.erase(gathered.first);
								this->adjectiveAntonymMap.erase(gathered.first);
								conceptDeleted = true;
							}
						}
					}
				}
			}
			if (!conceptDeleted)
			{
				it++;
			}
			else
			{
				conceptDeleted = false;
			}
		}
	}

	void ConceptNetCall::checkAdjectives(/*std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck*/)
	{
		std::map<QString, std::vector<QString>>::iterator it;
		bool conceptDeleted = false;
		for (it = this->adjectiveAntonymMap.begin(); it != this->adjectiveAntonymMap.end();)
		{
			if (this->adjectiveAntonymMap.find(it->first) == this->adjectiveAntonymMap.end())
			{
				it++;
				continue;
			}
			for (auto antonym : it->second)
			{
				if (this->adjectives.find(antonym) == this->adjectives.end())
				{
					continue;
				}
				if (this->adjectives.at(it->first)->weight < this->adjectives.at(antonym)->weight)
				{
					auto iterator = this->adjectives.find(it->first);
					if (iterator != this->adjectives.end())
					{
						std::cout << "ConceptNetCall: Antonym found: removing: " << it->first.toStdString()
								<< " keeping: " << antonym.toStdString() << std::endl;
						this->adjectives.erase(it->first);
						this->adjectiveAntonymMap.erase(it->first);
						conceptDeleted = true;
					}
				}
				else
				{
					auto iterator = this->adjectives.find(antonym);
					if (iterator != this->adjectives.end())
					{
						std::cout << "ConceptNetCall: Antonym found: removing: " << antonym.toStdString()
								<< " keeping: " << it->first.toStdString() << std::endl;
						this->adjectives.erase(antonym);
						this->adjectiveAntonymMap.erase(antonym);
						conceptDeleted = true;
					}
				}
			}
			if (!conceptDeleted)
			{
				it++;
			}
			else
			{
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
		for (auto adjective : toCheck)
		{
			this->currentConcept = adjective.first;
			QEventLoop loopIsA;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopIsA, SLOT(quit()));
			QUrl urlIsA("http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/IsA" + "&limit=1000");
			this->callUrl(urlIsA, this->checkNAM);
			loopIsA.exec();
			QEventLoop loopSynonym;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopSynonym, SLOT(quit()));
			QUrl urlSynonym(
					"http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/Synonym" + "&limit=1000");
			this->callUrl(urlSynonym, this->checkNAM);
			loopSynonym.exec();
			QEventLoop loopDefinedAs;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopDefinedAs, SLOT(quit()));
			QUrl urlDefinedAs(
					"http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/DefinedAs" + "&limit=1000");
			this->callUrl(urlDefinedAs, this->checkNAM);
			loopDefinedAs.exec();
			QEventLoop loopPartOf;
			this->connect(this, SIGNAL(closeLoopAdjectiveGathering()), &loopPartOf, SLOT(quit()));
			QUrl urlPartOf(
					"http://api.localhost/query?start=/c/en/" + adjective.first + "&rel=/r/PartOf" + "&limit=1000");
			this->callUrl(urlPartOf, this->checkNAM);
			loopPartOf.exec();
			this->conceptsToCheck.erase(adjective.first);
		}
		if (this->newConceptFound)
		{
			gatherConcepts(this->conceptsToCheck);
		}
	}

	void ConceptNetCall::collectConcepts(QNetworkReply* reply)
	{
		QString data = reply->readAll();
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
			if (cn5Edge->id.compare("BREAK") == 0)
			{
				break;
			}
			QString endTerm = edge["end"].toObject()["term"].toString();
			endTerm = trimTerm(endTerm);
			if (this->conceptsToCheck.find(endTerm) == this->conceptsToCheck.end()
					&& this->gatheredConcepts.find(endTerm) == this->gatheredConcepts.end()
					&& cn5Edge->secondConcept->senseLabel.compare("a") == 0)
			{
				this->conceptsToCheck.emplace(endTerm, cn5Edge);
				this->gatheredConcepts.emplace(
						endTerm, std::pair<QString, std::shared_ptr<ConceptNetEdge>>(this->currentConcept, cn5Edge));
				this->newConceptFound = true;
				for (auto p : this->gatherMap)
				{
					if (std::find(p.second->begin(), p.second->end(), this->currentConcept) != p.second->end())
					{
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
			return std::make_shared<ConceptNetEdge>(QString("BREAK"), QString(""), nullptr, nullptr, QString(""), -1.0);
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

	void ConceptNetCall::mapAntonyms(QNetworkReply* reply)
	{
//not current adjective and not already in
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
				break;
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
			if (endTerm != currentAdjective)
			{
				auto tmp = this->adjectiveAntonymMap.at(currentAdjective);
				if (std::find(tmp.begin(), tmp.end(), endTerm) == tmp.end())
				{
					this->adjectiveAntonymMap.at(currentAdjective).push_back(endTerm);
				}
			}
			if (startTerm != currentAdjective)
			{
				auto tmp = this->adjectiveAntonymMap.at(currentAdjective);
				if (std::find(tmp.begin(), tmp.end(), startTerm) == tmp.end())
				{
					this->adjectiveAntonymMap.at(currentAdjective).push_back(startTerm);
				}
			}
		}
		if (this->adjectiveAntonymMap.at(currentAdjective).size() == 0)
		{
			this->adjectiveAntonymMap.erase(currentAdjective);
		}
		emit closeLoop2();
	}

	void ConceptNetCall::collectAntonyms()
	{
		for (auto adjective : this->adjectives)
		{
			this->adjectiveAntonymMap.emplace(adjective.first, std::vector<QString>());
			this->currentAdjective = adjective.first;
			QUrl url("http://api.localhost/query?node=/c/en/" + adjective.first + "&rel=/r/Antonym" + "&limit=1000");
			this->callUrl(url, this->nam2);
			QEventLoop loop;
			this->connect(this, SIGNAL(closeLoop2()), &loop, SLOT(quit()));
			loop.exec();
		}
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

