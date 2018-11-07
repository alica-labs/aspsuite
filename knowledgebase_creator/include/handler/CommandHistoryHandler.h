/*
 * CommandHistoryHandler.h
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_
#define INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_

#include <QObject>
#include <memory>

namespace kbcr
{

class Command;
class KnowledgebaseCreator;
/**
 * Handles everything related to the command history
 */
class CommandHistoryHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param gui pointer to the main window handler
     */
    CommandHistoryHandler(KnowledgebaseCreator* gui);
    /**
     * Destructor
     */
    virtual ~CommandHistoryHandler();
    /**
     * Add command to history
     * @param cmd command to add
     */
    void addToCommandHistory(std::shared_ptr<Command> cmd);
    /**
     * Remove command from history
     * @param cmd command to be removed
     */
    void removeFromCommandHistory(std::shared_ptr<Command> cmd);

public slots:
    /**
     * Slot to fill command history table view
     */
    void fillCommandHistory();
    /**
     * Slot to redraw the command history
     */
    void drawHistoryTable();
    /**
     * Slot used by ctrl+z to call undo on the last element of the command history
     */
    void undoHistory();
    /**
     * Slot used by esc button to remove focus from the text area
     */
    void removeFocus();

private:
    /**
     * Pointer to main Gui handler
     */
    KnowledgebaseCreator* gui;
    /**
     * Add New Solver element to history
     */
    void addNewSolverCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Change Solver Settings element to history
     */
    void addChangeSolverSettingsCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Concept Net Query element to history
     */
    void addConceptNetQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Ground element to history
     */
    void addGroundCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Load Saved Program element to history
     */
    void addLoadSavedProgramCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Solve element to history
     */
    void addSolveCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Variable Query element to history
     */
    void addVariableQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Load Background Knowledge element to history
     */
    void addLoadBackgroundKnowledgeCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Facts Query element to history
     */
    void addFactsQueryCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Add element to history
     */
    void addAddCommandToHistory(std::shared_ptr<Command> cmd, int pos);
    /**
     * Add Assign External element to history
     */
    void addAssignExternalCommandToHistory(std::shared_ptr<Command> cmd, int pos);

    void addOfferServiceCommandToHistory(std::shared_ptr<Command> cmd, int pos);
};

} /* namespace kbcr */

#endif /* INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_ */
