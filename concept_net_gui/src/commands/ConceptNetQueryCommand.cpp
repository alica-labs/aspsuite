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
		this->connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(conceptNetCallFinished(QNetworkReply*)));
		this->connect(this, SIGNAL(jsonExtracted()), this, SLOT(extractASPPredicates()));
	}

	ConceptNetQueryCommand::~ConceptNetQueryCommand()
	{
		delete this->nam;
	}

	void ConceptNetQueryCommand::handleWrongInput()
	{
		auto pos = query.indexOf("(");
		auto parsedQuery = query.left(pos);
		QMessageBox* msgBox = new QMessageBox(this->gui);
		msgBox->setText(QString("Relation " + parsedQuery + " is not supported by ConceptNet 5!"));
		msgBox->setWindowModality(Qt::NonModal);
		QString relations;
		auto tmp = this->gui->getConceptNetBaseRealtions();
		for (int i = 0; i < tmp.size(); i++)
		{
			if (i % 2 == 0)
			{
				relations.append("\n");
			}
			relations.append(tmp.at(i));
			relations.append("\t");
			if (tmp.at(i).size() <= 10)
			{
				relations.append("\t");
			}
		}
		msgBox->setInformativeText(QString("Queries can be formulated using the following relations:" + relations));
		msgBox->setStandardButtons(QMessageBox::Ok);
		msgBox->setDefaultButton(QMessageBox::Ok);
		int ret = msgBox->exec();
		this->undo();
	}

	void ConceptNetQueryCommand::handleQuery()
	{
		auto pos = this->query.indexOf("(");
		auto relation = this->query.left(pos);
		if (this->query.contains("wildcard"))
		{
			auto wildcardPos = this->query.indexOf("wildcard");
			auto commaPos = this->query.indexOf(",");
			if(wildcardPos < commaPos)
			{
				auto concept = this->query.mid(commaPos + 1, this->query.size() - commaPos);
				concept = concept.left(concept.size() - 1).trimmed();
				QUrl url("http://api.localhost/query?end=/c/en/" + concept + "&rel=/r/" + relation);
				QNetworkRequest request(url);
				this->nam->get(request);
			}
			else
			{
				auto concept = this->query.mid(pos + 1, commaPos - pos).trimmed();
				concept = concept.left(concept.size() - 1);
				QUrl url("http://api.localhost/query?start=/c/en/" + concept + "&rel=/r/" + relation);
				QNetworkRequest request(url);
				this->nam->get(request);
			}
			// AtLocation(cup, wildcard)
		}
		else
		{
			auto concept = this->query.right(this->query.size() - pos - 1);
			concept = concept.left(concept.size() - 1);
			QUrl url("http://api.localhost/query?node=/c/en/" + concept + "&rel=/r/" + relation);
			QNetworkRequest request(url);
			this->nam->get(request);
		}
	}

	void ConceptNetQueryCommand::execute()
	{
		if (this->query.contains("(") && this->query.contains(")"))
		{
			if (!isConceptNetRealtion(this->query))
			{
				handleWrongInput();
				return;
			}
			else
			{
				handleQuery();
			}
		}
		else
		{
			QUrl url("http://api.localhost/c/en/" + this->query);
			QNetworkRequest request(url);
			this->nam->get(request);
		}
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
			//TODO not use label since it contains unecessary info
			QString endConcept = end["label"].toString();
			QString endLanguage = end["language"].toString();
			if (endLanguage != "en")
			{
//				cout << "not egnlish " << endLanguage.toStdString() << endl;
				continue;
			}
			QJsonObject start = edge["start"].toObject();
			QString startConcept = start["label"].toString();
			QString startLanguage = start["language"].toString();
			if (startLanguage != "en")
			{
//				cout << "not egnlish " << startLanguage.toStdString() << endl;
				continue;
			}
			QString relation = edge["rel"].toObject()["label"].toString();
			double weight = edge["weight"].toDouble();
			QJsonArray sources = edge["sources"].toArray();
			auto tmp = make_shared<ConceptNetEdge>(edgeId.toStdString(), startLanguage.toStdString(),
													startConcept.toStdString(), endConcept.toStdString(),
													relation.toStdString(), weight);
			for (int j = 0; j < sources.size(); j++)
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
		this->gui->getUi()->aspRuleTextArea->setText(createASPPredicates());
		this->gui->getUi()->conceptNetBtn->setEnabled(true);
		this->gui->getUi()->conceptNetBtn->setFocus();
	}

	std::string ConceptNetQueryCommand::conceptToASPPredicate(std::string concept)
	{
		vector<string> wordVector;
		std::size_t prev = 0, pos;
		while ((pos = concept.find_first_of(" ,.", prev)) != std::string::npos)
		{
			if (pos > prev)
			{
				wordVector.push_back(concept.substr(prev, pos - prev));
			}
			prev = pos + 1;
		}
		if (prev < concept.length())
		{
			wordVector.push_back(concept.substr(prev, std::string::npos));
		}
		if (wordVector.size() == 1)
		{
			return wordVector.at(0);
		}
		else
		{
			stringstream ss;
			wordVector.at(0)[0] = tolower(wordVector.at(0)[0]);
			ss << wordVector.at(0);
			for (int i = 1; i < wordVector.size(); i++)
			{
				wordVector.at(i)[0] = toupper(wordVector.at(i)[0]);
				ss << wordVector.at(i);
			}
			return ss.str();
		}
	}

	QString ConceptNetQueryCommand::createWeightedASPPredicates()
	{
		//TODO think about concept net weighting
		/**
		 * weights directly from conceptnet can not be used in asp predicates since they are double values containing a dot which violates
		 * the asp syntax
		 * a possible way is to multiply the weight with 100 and cast it to int keeping the first two digets after the komma and leaving the rest
		 * are weights even necessary? perhaps to use it during optimization
		 */
		stringstream ss;
		for (auto edge : this->currentConceptNetCall->edges)
		{
			string tmp = edge->relation;
			tmp[0] = tolower(tmp[0]);
			ss << tmp << "(" << conceptToASPPredicate(edge->firstConcept) << ", "
					<< conceptToASPPredicate(edge->secondConcept) << ", " << edge->weight << ", "
					<< edge->sources.size() << ")." << endl;
		}
		return QString(ss.str().c_str());
	}

	QString ConceptNetQueryCommand::createAvgWeightedASPPredicates()
	{
		//TODO think about avg weighting
		/**
		 * a high value is for example given for a cup holds a liquid which has a weight of 6.9 and 13 supporters
		 * which results in an average weight of 0.5... which is very low
		 * this can be caused by unreliable sources like word games and can lower the influence of good source, which are distinguished in weight and only the sum is given.
		 * perhaps do not use averaged weights
		 */
		stringstream ss;
		for (auto edge : this->currentConceptNetCall->edges)
		{
			string tmp = edge->relation;
			tmp[0] = tolower(tmp[0]);
			ss << tmp << "(" << conceptToASPPredicate(edge->firstConcept) << ", "
					<< conceptToASPPredicate(edge->secondConcept) << ", " << edge->weight / edge->sources.size() << ")."
					<< endl;
		}
		return QString(ss.str().c_str());
	}

	QString ConceptNetQueryCommand::createASPPredicates()
	{
		stringstream ss;
		for (auto edge : this->currentConceptNetCall->edges)
		{
			string tmp = edge->relation;
			tmp[0] = tolower(tmp[0]);
			ss << tmp << "(" << conceptToASPPredicate(edge->firstConcept) << ", "
					<< conceptToASPPredicate(edge->secondConcept) << ")." << endl;
		}
		return QString(ss.str().c_str());
	}

	bool ConceptNetQueryCommand::isConceptNetRealtion(QString query)
	{
		auto pos = query.indexOf("(");
		auto tmp = query.left(pos);
		for (auto relation : this->gui->getConceptNetBaseRealtions())
		{
			if (relation.compare(tmp) == 0)
			{
				return true;
			}
		}
		return false;
	}

} /* namespace cng */
