/*
 * ConceptNetQueryCommand.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#include "commands/ConceptNetQueryCommand.h"
#include "commands/GroundCommand.h"
#include "commands/SolveCommand.h"

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"

#include "gui/ConceptNetGui.h"
#include "gui/ModelSettingDialog.h"

#include "handler/CommandHistoryHandler.h"

#include "asp_solver/ASPSolver.h"

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
		this->connect(this->nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(conceptNetCallFinished(QNetworkReply*)));
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
		auto parsedQuery = this->query.left(this->query.indexOf("("));
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
		auto commaPos = this->query.indexOf(",");
		//cn5_Wildcard(concept, concept)
		if (this->query.contains("cn5_Wildcard"))
		{
			auto conceptRight = this->query.mid(commaPos + 1, this->query.size() - commaPos);
			auto conceptLeft = this->query.mid(pos + 1, commaPos - pos).trimmed();
			conceptRight = conceptRight.left(conceptRight.size() - 1).trimmed();
			conceptLeft = conceptLeft.left(conceptLeft.size() - 1).trimmed();
			conceptRight = conceptRight.mid(this->prefixLength, conceptRight.length() - this->prefixLength);
			conceptLeft = conceptLeft.mid(this->prefixLength, conceptLeft.length() - this->prefixLength);
			QUrl url("http://api.localhost/query?start=/c/en/" + conceptLeft + "&end=/c/en/" + conceptRight);
			callUrl(url);
		}
		else if (this->query.contains("wildcard") && !this->query.contains("cn5_Wildcard"))
		{
			auto wildcardPos = this->query.indexOf("wildcard");
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
		this->gui->enableGui(false);
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
			if (find(this->currentConceptNetCall->concepts.begin(), this->currentConceptNetCall->concepts.end(),
						endTerm.toStdString()) == this->currentConceptNetCall->concepts.end())
			{
				this->currentConceptNetCall->concepts.push_back(endTerm.toStdString());
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
			if (find(this->currentConceptNetCall->concepts.begin(), this->currentConceptNetCall->concepts.end(),
						startTerm.toStdString()) == this->currentConceptNetCall->concepts.end())
			{
				this->currentConceptNetCall->concepts.push_back(startTerm.toStdString());
			}
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
		if (!nextPage.isEmpty())
		{
			QUrl url("http://api.localhost" + nextPage);
			callUrl(url);
		}
		else
		{
#ifdef ConceptNetQueryCommandDebug
			cout << this->currentConceptNetCall->toString();
#endif
			// start json processing
//TODO remove comment
//			this->currentConceptNetCall->findAdjectives();
			emit jsonExtracted();
		}

	}

	void ConceptNetQueryCommand::extractASPPredicates()
	{
		QString programSection = "#program cn5_metaKnowledge";
		QString program = programSection;
		program.append(".\n");
		auto tmp = createASPPredicates();
		program.append(tmp);
		std::shared_ptr<GroundCommand> gc = std::make_shared<GroundCommand>(this->gui, program);
		gc->execute();
		std::shared_ptr<SolveCommand> sc = std::make_shared<SolveCommand>(this->gui);
		sc->execute();
		auto pgmMap = extractBackgroundKnowledgePrograms(tmp);
		for (auto pair : pgmMap)
		{
			this->gui->getSolver()->add(programSection.toStdString(), {}, pair.second.toStdString());
			this->gui->getUi()->programLabel->setText(
					this->gui->getUi()->programLabel->text().append("\n").append(pair.second).append("\n"));
		}
		this->gui->enableGui(true);
		this->gui->getUi()->conceptNetBtn->setFocus();
	}

	QString ConceptNetQueryCommand::conceptToASPPredicate(QString concept)
	{
		//  remove chars not supported by asp (UTF-8 or syntax symbols)
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
		if (concept.contains("è"))
		{
			concept.replace("è", "e");
		}
		if (concept.contains("é"))
		{
			concept.replace("é", "e");
		}
		if(concept.contains("æ"))
		{
			concept.replace("æ", "ae");
		}
		return concept;
	}

	QString ConceptNetQueryCommand::createASPPredicates()
	{
		QString ret = "";
		for (auto edge : this->currentConceptNetCall->edges)
		{
			if (edge->weight < this->gui->modelSettingsDialog->getMinCn5Weight())
			{
				continue;
			}
			QString tmp = "";
			tmp.append(this->prefix).append(edge->relation);
			tmp.append("(").append(this->prefix).append(conceptToASPPredicate(edge->firstConcept)).append(", ").append(
					this->prefix).append(
					conceptToASPPredicate(edge->secondConcept).append(", ").append(
							QString::number((int)(edge->weight * edge->sources.size())))).append(").\n");
			ret.append(tmp);
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

	std::map<QString, QString> ConceptNetQueryCommand::extractBackgroundKnowledgePrograms(QString conceptNetProgram)
	{
		std::map<QString, QString> ret;
		std::vector<QString> addedRelations;
		for (auto edge : this->currentConceptNetCall->edges)
		{
			if (edge->weight < this->gui->modelSettingsDialog->getMinCn5Weight())
			{
				continue;
			}
			if (edge->firstConcept.at(0).isDigit() || edge->secondConcept.at(0).isDigit())
			{
				std::cout << "ConceptNetQueryCommand: SKipping edge:" << edge->firstConcept.toStdString() << " " << edge->relation.toStdString()
						<< " " << edge->secondConcept.toStdString() << " since one concept starts with a number!" << std::endl;
				continue;
			}
			QString tmpRel = edge->relation;
			tmpRel[0] = tmpRel[0].toLower();
			auto it = find(addedRelations.begin(), addedRelations.end(), tmpRel);
			if (it == addedRelations.end())
			{
				addedRelations.push_back(tmpRel);
				QString pgm = "#program cn5Knowledge(n,m).\n";
				pgm.append("#external -").append(tmpRel).append("(n, m).\n");
				pgm.append(tmpRel).append("(n, m) :- not -").append(tmpRel).append("(n, m), ").append(
						conceptToASPPredicate(edge->firstConcept)).append("(n), ").append(
						conceptToASPPredicate(edge->secondConcept)).append("(m)").append(".\n");
				ret.emplace(tmpRel, pgm);

			}
			else
			{
				ret.at(tmpRel).append(tmpRel).append("(n, m) :- not -").append(tmpRel).append("(n, m), ").append(
						conceptToASPPredicate(edge->firstConcept)).append("(n), ").append(
						conceptToASPPredicate(edge->secondConcept)).append("(m)").append(".\n");
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

} /* namespace cng */
