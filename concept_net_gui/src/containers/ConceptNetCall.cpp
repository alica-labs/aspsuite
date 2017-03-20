/*
 * ConceptNetCall.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"

#include <string>
#include <sstream>
#include <iostream>

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace cng
{

	ConceptNetCall::ConceptNetCall(QString id)
	{
		this->id = id;
		this->nextEdgesPage = nextEdgesPage;
		this->nam = new QNetworkAccessManager(this);
		this->currentAdjectiveIndex = 0;
		this->connect(this->nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(adjectiveChecked(QNetworkReply*)));
		this->connect(this, SIGNAL(nextAdjective()), this, SLOT(findAdjectives()));
	}

	ConceptNetCall::~ConceptNetCall()
	{
	}

	std::string ConceptNetCall::toString()
	{
		std::stringstream ss;
		ss << "ConceptNetCall:" << std::endl;
		ss << "ID: " << this->id.toStdString() << std::endl;
//		ss << "Edges:" << std::endl;
//		for (auto edge : this->edges)
//		{
//			ss << edge->toString();
//		}
//		ss << "NextEdgePage: " << this->nextEdgesPage.toStdString() << std::endl;
		for (auto concept : this->concepts)
		{
			ss << concept << " ";
		}
		ss << std::endl;
		return ss.str();
	}

	void ConceptNetCall::findAdjectives()
	{
			if(this->currentAdjectiveIndex >= this->concepts.size())
			{
				for(auto adj : this->adjectives)
				{
					std::cout << adj << std::endl;
				}
				return;
			}
			auto concept = this->concepts.at(this->currentAdjectiveIndex);
			QString urlString = "http://api.localhost/query?node=/c/en/";
			urlString.append(concept.c_str()).append("/a");
			QUrl url(urlString);
			QNetworkRequest request(url);
			this->nam->get(request);
	}

	void ConceptNetCall::adjectiveChecked(QNetworkReply* reply)
	{
		//get data
		QString data = reply->readAll();
		//remove html part
		std::string fullData = data.toStdString();
		auto start = fullData.find("{\"@context\":");
		auto end = fullData.find("</script>");
		fullData = fullData.substr(start, end - start);
		// parse json
		auto jsonString = QString(fullData.c_str()).toUtf8();
		QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
		QString id = jsonDoc.object()["@id"].toString();
		// save next page form following get request
		QString nextPage = jsonDoc.object()["view"].toObject()["nextPage"].toString();
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		if(edges.size() > 0)
		{
			this->adjectives.push_back(this->concepts.at(this->currentAdjectiveIndex));
		}
		this->currentAdjectiveIndex++;
		emit nextAdjective();

	}

} /* namespace cng */

