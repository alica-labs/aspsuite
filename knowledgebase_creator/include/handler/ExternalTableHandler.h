#pragma once

#include <QObject>
#include <QTableWidget>

#include <memory>

namespace kbcr
{

class KnowledgebaseCreator;
/**
 * Handles the external statement table
 */
class ExternalTableHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    ExternalTableHandler(KnowledgebaseCreator* gui);
    /**
     * Destructor
     */
    virtual ~ExternalTableHandler();

public slots:
    /**
     * Slot to fill command history table view
     */
    void fillExternalTable();
    /**
     * Slot to redraw the command history
     */
    void drawExternalTable();
    /**
     * Slot for filling the external label
     */
    void updateExternalLabel(QTableWidgetItem * item);
private:
    /**
     * Pointer to main Gui handler
     */
    KnowledgebaseCreator* gui;
};

} /* namespace kbcr */

