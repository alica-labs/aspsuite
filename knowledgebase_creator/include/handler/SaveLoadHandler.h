#pragma once

#include <QObject>

#include <memory>

namespace kbcr
{
class LoadSavedProgramCommand;
class KnowledgebaseCreator;
/**
 * Handles Save and Load methods
 */
class SaveLoadHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param gui pointer to main window handler
     */
    SaveLoadHandler(KnowledgebaseCreator* gui);
    virtual ~SaveLoadHandler();

    std::shared_ptr<LoadSavedProgramCommand> currentLoadCmd;

public slots:
    /**
     * Save command history in Json
     */
    void saveProgram();
    /**
     * Save models depending on settings
     */
    void saveModels();
    /**
     * Load saved command history from Json
     */
    void loadProgram();
    /**
     * Load Back Ground Knowledge File
     */
    void loadBackgroundKnowledge();

private:
    /**
     * Pointer to the main window handler
     */
    KnowledgebaseCreator* gui;
};

} /* namespace kbcr */

