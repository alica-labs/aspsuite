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
		this->query = query.trimmed();
		if (this->query[this->query.size() - 1] == '.')
		{
			this->query = this->query.remove(this->query.size() - 1, 1);
		}
		this->currentConceptNetCall = nullptr;
		this->nam = new QNetworkAccessManager(this);
		this->prefix = QString("cn5_");
		this->prefixLength = this->prefix.length();
		// connect signals form handling the http requests
		this->connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(conceptNetCallFinished(QNetworkReply*)));
		this->connect(this, SIGNAL(jsonExtracted()), this, SLOT(extractASPPredicates()));
	}

	ConceptNetQueryCommand::~ConceptNetQueryCommand()
	{
		delete this->nam;
		delete this->currentConceptNetCall;
	}

	void ConceptNetQueryCommand::handleWrongInput()
	{
		// parse wrong input
		auto parsedQuery = query.left(query.indexOf("("));
		//prepare MessageBox
		QMessageBox msgBox;
		msgBox.setText(QString("Relation " + parsedQuery + " is not supported by ConceptNet 5!"));
		msgBox.setWindowModality(Qt::NonModal);
		// print possible base relations
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
		msgBox.setInformativeText(QString("Queries can be formulated using the following relations:" + relations));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		// show message box
		int ret = msgBox.exec();
		//remove from command history
		this->undo();
	}

	QString ConceptNetQueryCommand::expandConceptNetPredicate(QString predicate)
	{
		QString ret = "";
		ret.append(predicate).append(" :- not ").append("-").append(predicate).append(".\n");
		return ret;
	}

	void ConceptNetQueryCommand::callUrl(QUrl url)
	{
		QNetworkRequest request(url);
		this->nam->get(request);
	}

	void ConceptNetQueryCommand::handleQuery()
	{
		//handle different queries
		auto pos = this->query.indexOf("(");

		auto relation = this->query.mid(this->prefixLength, pos - this->prefixLength);
		std::cout << relation.toStdString() << std::endl;
		if (this->query.contains("wildcard"))
		{
			auto wildcardPos = this->query.indexOf("wildcard");
			auto commaPos = this->query.indexOf(",");
			// relation(wildcard, concept)
			if (wildcardPos < commaPos)
			{
				auto concept = this->query.mid(commaPos + 1, this->query.size() - commaPos);
				concept = concept.left(concept.size() - 1).trimmed();
				concept = concept.mid(this->prefixLength, concept.length() - this->prefixLength);
				QUrl url("http://api.localhost/query?end=/c/en/" + concept + "&rel=/r/" + relation);
				callUrl(url);
			}
			// relation(concept, wildcard)
			else
			{
				auto concept = this->query.mid(pos + 1, commaPos - pos).trimmed();
				concept = concept.left(concept.size() - 1);
				concept = concept.mid(this->prefixLength, concept.length() - this->prefixLength);
				QUrl url("http://api.localhost/query?start=/c/en/" + concept + "&rel=/r/" + relation);
				callUrl(url);
			}
		}
		//relation(concept)
		else
		{
			auto concept = this->query.right(this->query.size() - pos - 1);
			concept = concept.left(concept.size() - 1);
			concept = concept.mid(this->prefixLength, concept.length() - this->prefixLength);
			QUrl url("http://api.localhost/query?node=/c/en/" + concept + "&rel=/r/" + relation);
			callUrl(url);
		}
	}

	void ConceptNetQueryCommand::execute()
	{
		if (!this->query.startsWith(this->prefix))
		{
			QMessageBox msgBox;
			msgBox.setText(QString("ConceptNet 5 queries have to start with the prefix \"cn5_\"."));
			msgBox.setWindowModality(Qt::NonModal);
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			// show message box
			int ret = msgBox.exec();
			this->undo();
			return;
		}
		if (this->query.contains("(") && this->query.contains(")"))
		{
			//wrong input relation
			if (!isConceptNetRealtion(this->query))
			{
				handleWrongInput();
				return;
			}
			// query with given relation
			else
			{
				handleQuery();
			}
		}
		//query without relation
		else
		{
			QUrl url(
					"http://api.localhost/c/en/"
							+ this->query.mid(this->prefixLength, this->query.length() - this->prefixLength));
			callUrl(url);
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

	QString ConceptNetQueryCommand::trimTerm(QString term)
	{
		auto pos = term.lastIndexOf("/");
		return term.right(term.length() - pos - 1);
	}

	void ConceptNetQueryCommand::conceptNetCallFinished(QNetworkReply* reply)
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
#ifdef ConceptNetQueryCommandDebug
		cout << id.toStdString() << endl;
		cout << nextPage.toStdString() << endl;
#endif
		// only null in first query
		if (this->currentConceptNetCall == nullptr)
		{
			this->currentConceptNetCall = new ConceptNetCall(id);
		}
		this->currentConceptNetCall->nextEdgesPage = nextPage;
		// extract edges
		QJsonArray edges = jsonDoc.object()["edges"].toArray();
		for (int i = 0; i < edges.size(); i++)
		{
			QJsonObject edge = edges[i].toObject();
			//end of edge
			QString edgeId = edge["@id"].toString();
			QJsonObject end = edge["end"].toObject();
			QString endLanguage = end["language"].toString();
			// skip non English
			if (endLanguage != "en")
			{
				continue;
			}
			QString endTerm = end["term"].toString();
			endTerm = trimTerm(endTerm);
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
			QString relation = edge["rel"].toObject()["label"].toString();
			double weight = edge["weight"].toDouble();
			// sources
			QJsonArray sources = edge["sources"].toArray();
			auto tmp = std::make_shared<ConceptNetEdge>(edgeId, startLanguage, startTerm, endTerm, relation, weight);
			for (int j = 0; j < sources.size(); j++)
			{
				tmp->sources.push_back(sources[j].toObject()["contributor"].toString());
			}
			this->currentConceptNetCall->edges.push_back(tmp);
		}
#ifdef ConceptNetQueryCommandDebug
		cout << this->currentConceptNetCall->toString();
#endif
		if (!nextPage.isEmpty())
		{
			QUrl url("http://api.localhost" + nextPage);
			callUrl(url);
		}
		else
		{
			// start json processing
			emit jsonExtracted();
		}

	}

	void ConceptNetQueryCommand::extractASPPredicates()
	{
		this->gui->getUi()->aspRuleTextArea->setText(createASPPredicates());
		this->gui->getUi()->conceptNetBtn->setEnabled(true);
		this->gui->getUi()->conceptNetBtn->setFocus();
	}

	QString ConceptNetQueryCommand::conceptToASPPredicate(QString concept)
	{
		//  remove chars not supported by asp
		if (concept.contains('.'))
		{
			concept.replace('.', '_');
		}
		if (concept.contains(','))
		{
			concept.replace(',', '_');
		}
		if (concept.contains(' '))
		{
			concept.replace(' ', '_');
		}
		return concept;
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
		QString ret = "";
		for (auto edge : this->currentConceptNetCall->edges)
		{
			QString tmp = edge->relation;
			tmp[0] = tmp[0].toLower();
			ret.append(tmp).append("(").append(conceptToASPPredicate(edge->firstConcept)).append(", ").append(
					conceptToASPPredicate(edge->secondConcept)).append(", ").append(
					QString::number((int)(edge->weight * 100))).append(", ").append(
					QString::number(edge->sources.size())).append(").\n");
		}
		return ret;
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
		QString ret = "";
		for (auto edge : this->currentConceptNetCall->edges)
		{
			QString tmp = edge->relation;
			tmp[0] = tmp[0].toLower();
			ret.append(tmp).append("(").append(conceptToASPPredicate(edge->firstConcept)).append(", ").append(
					conceptToASPPredicate(edge->secondConcept)).append(", ").append(
					QString::number((int)(edge->weight / edge->sources.size()))).append(").\n");
		}
		return ret;
	}

	QString ConceptNetQueryCommand::createASPPredicates()
	{
		QString ret = "";
		for (auto edge : this->currentConceptNetCall->edges)
		{
//			QString tmp = edge->relation;
//			tmp[0] = tmp[0].toLower();
//			ret.append(tmp).append("(").append(conceptToASPPredicate(edge->firstConcept)).append(", ").append(
//					conceptToASPPredicate(edge->secondConcept)).append(").\n");
			QString tmp = "";
			tmp.append(this->prefix).append(edge->relation);
			tmp.append("(").append(this->prefix).append(conceptToASPPredicate(edge->firstConcept)).append(", ").append(
					this->prefix).append(conceptToASPPredicate(edge->secondConcept)).append(").\n");
//			ret.append(expandConceptNetPredicate(tmp));
			ret.append(tmp);
			QString tmpRel = edge->relation;
			tmpRel[0] = tmpRel[0].toLower();
			ret.append(tmpRel).append("(X, Y) :- not -").append(tmpRel).append("(X, Y), ").append(
					conceptToASPPredicate(edge->firstConcept)).append("(X), ").append(conceptToASPPredicate(edge->secondConcept)).append("(Y)").append(".\n");

		}
		return ret;
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
