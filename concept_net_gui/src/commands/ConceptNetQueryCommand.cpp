/*
 * ConceptNetQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/ConceptNetQueryCommand.h"

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"

#include "gui/ConceptNetGui.h"

#include "handler/CommandHistoryHandler.h"

#include <ui_conceptnetgui.h>

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>

namespace cng
{

	ConceptNetQueryCommand::ConceptNetQueryCommand(ConceptNetGui* gui, QString query)
	{
		this->type = "Concept Net";
		this->gui = gui;
		this->query = query;
		this->currentConceptNetCall = nullptr;
		this->nam = new QNetworkAccessManager(this);
		this->connect(nam, SIGNAL(finished(QNetworkReply*)), this,
				SLOT(conceptNetCallFinished(QNetworkReply*)));
		this->connect(this, SIGNAL(jsonExtracted()), this, SLOT(extractASPPredicates()));
	}

	ConceptNetQueryCommand::~ConceptNetQueryCommand()
	{
		delete this->nam;
	}

	void ConceptNetQueryCommand::execute()
	{
		QUrl url("http://api.localhost/c/en/" + this->query);
		QNetworkRequest request(url);
		this->nam->get(request);
		this->gui->chHandler->addToCommandHistory(shared_from_this());
		this->gui->getUi()->conceptNetBtn->setEnabled(false);
	}

	void ConceptNetQueryCommand::undo()
	{
		this->gui->chHandler->removeFromCommandHistory(shared_from_this());
	}

	QJsonObject ConceptNetQueryCommand::toJSON()
	{
		QJsonObject ret;
		ret["type"] = "Concept Net";
		ret["queryString"] = this->query;
		return ret;
	}

	void ConceptNetQueryCommand::conceptNetCallFinished(QNetworkReply* reply)
	{
		QString data = reply->readAll();
		string fullData = data.toStdString();
		auto start = fullData.find("{\"@context\":");
		auto end = fullData.find("</script>");
		fullData = fullData.substr(start, end - start);
		//		cout << "Data: " << fullData << endl;
		auto jsonString = QString(fullData.c_str()).toUtf8();
		//		cout << "DataJson: " << jsonString.toStdString() << endl;
		QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonString));
		QString id = jsonDoc.object()["@id"].toString();
		QString nextPage = jsonDoc.object()["view"].toObject()["nextPage"].toString();
#ifdef ConceptNetQueryCommandDebug
		cout << id.toStdString() << endl;
		cout << nextPage.toStdString() << endl;
#endif
		if (this->currentConceptNetCall == nullptr)
		{
			this->currentConceptNetCall = make_shared<ConceptNetCall>(id.toStdString());
		}
		this->currentConceptNetCall->nextEdgesPage = nextPage.toStdString();
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		for (int i = 0; i < edges.size(); i++)
		{
			QJsonObject edge = edges[i].toObject();
			QString edgeId = edge["@id"].toString();
			QJsonObject end = edge["end"].toObject();
			QString endConcept = end["label"].toString();
			QString endLanguage = end["language"].toString();
			if (endLanguage != "en")
			{
				continue;
			}
			QJsonObject start = edge["start"].toObject();
			QString startConcept = start["label"].toString();
			QString startLanguage = start["language"].toString();
			if (startLanguage != "en")
			{
				continue;
			}
			QString relation = edge["rel"].toObject()["label"].toString();
			double weight = edge["weight"].toDouble();
			QJsonArray sources = edge["sources"].toArray();
			auto tmp = make_shared<ConceptNetEdge>(edgeId.toStdString(), startLanguage.toStdString(),
													startConcept.toStdString(), endConcept.toStdString(),
													relation.toStdString(), weight);
			for(int j = 0; j < sources.size(); j++)
			{
				tmp->sources.push_back(sources[j].toObject()["contributor"].toString().toStdString());
			}
			this->currentConceptNetCall->edges.push_back(tmp);
		}
#ifdef ConceptNetQueryCommandDebug
		cout << this->currentConceptNetCall->toString();
#endif
		if (!nextPage.isEmpty())
		{
			QUrl url("http://api.localhost" + nextPage);
			QNetworkRequest request(url);
			this->nam->get(request);
		}
		else
		{
			emit jsonExtracted();
		}

	}

	void ConceptNetQueryCommand::extractASPPredicates()
	{
		this->gui->getUi()->conceptNetBtn->setEnabled(true);
		stringstream ss;
		for(auto edge : this->currentConceptNetCall->edges)
		{
			string tmp = edge->relation;
			tmp[0] = tolower(tmp[0]);
			string first = edge->firstConcept;
			if(first.find(".") != string::npos)
			{
				first.erase(remove(first.begin(), first.end(), '.'), first.end());
			}
			if(first.find(",") != string::npos)
			{
				first.erase(remove(first.begin(), first.end(), ','), first.end());
			}
			string second = edge->secondConcept;
			if(second.find(".") != string::npos)
			{
				second.erase(remove(second.begin(), second.end(), '.'), second.end());
			}
			if(second.find(",") != string::npos)
			{
				second.erase(remove(second.begin(), second.end(), ','), second.end());
			}
			ss << tmp << "(" << first << ", " << second << ")." << endl;
			//TODO remove spaces, camel case ?
		}
		this->gui->getUi()->aspRuleTextArea->setText(QString(ss.str().c_str()));
	}

} /* namespace cng */
