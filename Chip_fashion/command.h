#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "canvasscene.h"
#include "cellitem_new.h"
#include "chipmanager.h"
#include "connectionline.h"
#include <QSizeF>

class AddRectangleCommand : public QUndoCommand
{
public:
    AddRectangleCommand(CanvasScene* scene, const QPointF& pos, ChipManager* chipManager, QUndoStack* undoStack)
        : QUndoCommand("添加矩形"), m_scene(scene), m_pos(pos), m_chipManager(chipManager), m_undoStack(undoStack)
    {
        // 使用ChipManager创建芯片，确保统一的自增编号
        m_cellItem = m_chipManager->createNewChip(m_pos, QSizeF(150, 100));
        m_chipNumber = m_chipManager->getChipCounter(); // 保存当前编号用于撤销
    }

    void undo() override
    {
        if (m_cellItem && m_scene && m_chipManager) {
            m_scene->removeItem(m_cellItem);
            // 注意：这里不需要修改chipManager的计数器，因为撤销不应该影响下一个芯片的编号
            qDebug() << "撤销添加矩形 at" << m_pos << ", InstanceName:" << m_cellItem->getInstanceName();
        }
    }

    void redo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->addCellItem(m_cellItem);
            qDebug() << "重做添加矩形 at" << m_pos << ", InstanceName:" << m_cellItem->getInstanceName();
        }
    }

private:
    CanvasScene* m_scene;
    CellItem* m_cellItem;
    QPointF m_pos;
    ChipManager* m_chipManager; // 使用ChipManager管理计数器
    QUndoStack* m_undoStack;
    int m_chipNumber; // 保存芯片编号用于调试
};

class DeleteRectangleCommand : public QUndoCommand
{
public:
    DeleteRectangleCommand(CanvasScene* scene, CellItem* cellItem, QUndoStack* undoStack)
        : QUndoCommand("删除矩形"), m_scene(scene), m_cellItem(cellItem), m_undoStack(undoStack)
    {
        m_pos = cellItem->pos();
    }

    void undo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->addItem(m_cellItem);
            m_cellItem->setPos(m_pos);
            m_scene->update();
        }
    }

    void redo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->removeItem(m_cellItem);
            m_scene->update();
        }
    }

private:
    CanvasScene* m_scene;
    CellItem* m_cellItem;
    QPointF m_pos;
    QUndoStack* m_undoStack;
};

class AddPinCommand : public QUndoCommand
{
public:
    AddPinCommand(CellItem* cellItem, const QString& side, qreal percentage,
                  qreal size, const QString& id, qreal x, qreal y)
        : QUndoCommand("添加引脚"), m_cellItem(cellItem), m_side(side),
        m_percentage(percentage), m_size(size), m_id(id), m_x(x), m_y(y)
    {
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->removeConnector(m_id);
            qDebug() << "撤销添加引脚:" << m_id;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->addConnector(m_side, m_percentage, m_size, m_id, m_x, m_y);
            qDebug() << "重做添加引脚:" << m_id;
        }
    }

private:
    CellItem* m_cellItem;
    QString m_side;
    qreal m_percentage;
    qreal m_size;
    QString m_id;
    qreal m_x;
    qreal m_y;
};

// 移动芯片命令
class MoveCellCommand : public QUndoCommand
{
public:
    MoveCellCommand(CellItem* cellItem, const QPointF& oldPos, const QPointF& newPos)
        : QUndoCommand("移动芯片"), m_cellItem(cellItem), m_oldPos(oldPos), m_newPos(newPos)
    {
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->m_positionUpdateInProgress = true;
            m_cellItem->setPos(m_oldPos);
            m_cellItem->m_lastRecordedPosition = m_oldPos;
            m_cellItem->m_positionUpdateInProgress = false;
            qDebug() << "撤销移动芯片到:" << m_oldPos;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->m_positionUpdateInProgress = true;
            m_cellItem->setPos(m_newPos);
            m_cellItem->m_lastRecordedPosition = m_newPos;
            m_cellItem->m_positionUpdateInProgress = false;
            qDebug() << "重做移动芯片到:" << m_newPos;
        }
    }

private:
    CellItem* m_cellItem;
    QPointF m_oldPos;
    QPointF m_newPos;
};

// 修改芯片尺寸命令
class ResizeCellCommand : public QUndoCommand
{
public:
    ResizeCellCommand(CellItem* cellItem, const QSizeF& oldSize, const QSizeF& newSize)
        : QUndoCommand("调整芯片尺寸"), m_cellItem(cellItem), m_oldSize(oldSize), m_newSize(newSize)
    {
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->setSize(m_oldSize);
            qDebug() << "撤销调整芯片尺寸到:" << m_oldSize;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->setSize(m_newSize);
            qDebug() << "重做调整芯片尺寸到:" << m_newSize;
        }
    }

private:
    CellItem* m_cellItem;
    QSizeF m_oldSize;
    QSizeF m_newSize;
};

// 修改芯片属性命令
class ChangeCellPropertiesCommand : public QUndoCommand
{
public:
    ChangeCellPropertiesCommand(CellItem* cellItem,
                                const QString& oldMacroName, const QString& newMacroName,
                                const QString& oldInstanceName, const QString& newInstanceName)
        : QUndoCommand("修改芯片属性"), m_cellItem(cellItem),
        m_oldMacroName(oldMacroName), m_newMacroName(newMacroName),
        m_oldInstanceName(oldInstanceName), m_newInstanceName(newInstanceName)
    {
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->setMacroName(m_oldMacroName);
            m_cellItem->setInstanceName(m_oldInstanceName);
            qDebug() << "撤销修改芯片属性:" << m_oldMacroName << m_oldInstanceName;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->setMacroName(m_newMacroName);
            m_cellItem->setInstanceName(m_newInstanceName);
            qDebug() << "重做修改芯片属性:" << m_newMacroName << m_newInstanceName;
        }
    }

private:
    CellItem* m_cellItem;
    QString m_oldMacroName;
    QString m_newMacroName;
    QString m_oldInstanceName;
    QString m_newInstanceName;
};

// 添加连线命令
class AddConnectionCommand : public QUndoCommand
{
public:
    AddConnectionCommand(CanvasScene* scene, CellItem* sourceCell, const QString& sourcePin,
                         CellItem* targetCell, const QString& targetPin)
        : QUndoCommand("添加连线"), m_scene(scene), m_sourceCell(sourceCell), m_sourcePin(sourcePin),
        m_targetCell(targetCell), m_targetPin(targetPin), m_connectionLine(nullptr)
    {
        // 查找连接器信息
        auto sourceConnectors = m_sourceCell->getConnectors();
        auto targetConnectors = m_targetCell->getConnectors();

        for (const auto& conn : sourceConnectors) {
            if (conn.id == sourcePin) {
                m_sourceConnector = conn;
                break;
            }
        }

        for (const auto& conn : targetConnectors) {
            if (conn.id == targetPin) {
                m_targetConnector = conn;
                break;
            }
        }
    }

    void undo() override
    {
        if (m_connectionLine && m_scene) {
            m_scene->removeConnectionLine(m_connectionLine);
            m_sourceCell->removeConnection(m_targetCell, m_sourcePin, m_targetPin);
            qDebug() << "撤销添加连线:" << m_sourcePin << "到" << m_targetPin;
        }
    }

    void redo() override
    {
        if (m_scene && m_sourceCell && m_targetCell) {
            m_sourceCell->addConnection(m_targetCell, m_sourcePin, m_targetPin);
            m_connectionLine = new ConnectionLine(m_sourceCell, m_sourceConnector, m_targetCell, m_targetConnector);
            m_scene->addConnectionLine(m_connectionLine);
            qDebug() << "重做添加连线:" << m_sourcePin << "到" << m_targetPin;
        }
    }

private:
    CanvasScene* m_scene;
    CellItem* m_sourceCell;
    CellItem* m_targetCell;
    QString m_sourcePin;
    QString m_targetPin;
    CellItem::Connector m_sourceConnector;
    CellItem::Connector m_targetConnector;
    ConnectionLine* m_connectionLine;
};

// 删除连线命令
class RemoveConnectionCommand : public QUndoCommand
{
public:
    RemoveConnectionCommand(CanvasScene* scene, ConnectionLine* connectionLine)
        : QUndoCommand("删除连线"), m_scene(scene), m_connectionLine(connectionLine)
    {
        // 保存连线信息
        if (m_connectionLine) {
            m_sourceCell = m_connectionLine->getSourceCell();
            m_targetCell = m_connectionLine->getTargetCell();
            m_sourceConnector = m_connectionLine->getSourceConnector();
            m_targetConnector = m_connectionLine->getTargetConnector();
        }
    }

    void undo() override
    {
        if (m_scene && m_sourceCell && m_targetCell) {
            m_sourceCell->addConnection(m_targetCell, m_sourceConnector.id, m_targetConnector.id);
            m_connectionLine = new ConnectionLine(m_sourceCell, m_sourceConnector, m_targetCell, m_targetConnector);
            m_scene->addConnectionLine(m_connectionLine);
            qDebug() << "撤销删除连线:" << m_sourceConnector.id << "到" << m_targetConnector.id;
        }
    }

    void redo() override
    {
        if (m_connectionLine && m_scene) {
            m_scene->removeConnectionLine(m_connectionLine);
            m_sourceCell->removeConnection(m_targetCell, m_sourceConnector.id, m_targetConnector.id);
            qDebug() << "重做删除连线:" << m_sourceConnector.id << "到" << m_targetConnector.id;
        }
    }

private:
    CanvasScene* m_scene;
    ConnectionLine* m_connectionLine;
    CellItem* m_sourceCell;
    CellItem* m_targetCell;
    CellItem::Connector m_sourceConnector;
    CellItem::Connector m_targetConnector;
};

// 修改引脚属性命令
class ModifyPinCommand : public QUndoCommand
{
public:
    ModifyPinCommand(CellItem* cellItem, const QString& pinId,
                     const CellItem::Connector& oldConnector, const CellItem::Connector& newConnector)
        : QUndoCommand("修改引脚"), m_cellItem(cellItem), m_pinId(pinId),
        m_oldConnector(oldConnector), m_newConnector(newConnector)
    {
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->removeConnector(m_pinId);
            m_cellItem->addConnector(m_oldConnector.side, m_oldConnector.percentage, 1.0,
                                     m_oldConnector.id, m_oldConnector.x, m_oldConnector.y);
            qDebug() << "撤销修改引脚:" << m_pinId;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->removeConnector(m_pinId);
            m_cellItem->addConnector(m_newConnector.side, m_newConnector.percentage, 1.0,
                                     m_newConnector.id, m_newConnector.x, m_newConnector.y);
            qDebug() << "重做修改引脚:" << m_pinId;
        }
    }

private:
    CellItem* m_cellItem;
    QString m_pinId;
    CellItem::Connector m_oldConnector;
    CellItem::Connector m_newConnector;
};

class RemovePinCommand : public QUndoCommand
{
public:
    RemovePinCommand(CellItem* cellItem, const QString& pinId)
        : QUndoCommand("删除引脚"), m_cellItem(cellItem), m_pinId(pinId)
    {
        // 保存引脚信息以便撤销
        if (m_cellItem) {
            auto connectors = m_cellItem->getConnectors();
            for (const auto& conn : connectors) {
                if (conn.id == pinId) {
                    m_side = conn.side;
                    m_percentage = conn.percentage;
                    m_x = conn.x;
                    m_y = conn.y;
                    break;
                }
            }
        }
    }

    void undo() override
    {
        if (m_cellItem) {
            m_cellItem->addConnector(m_side, m_percentage, 1.0, m_pinId, m_x, m_y);
            qDebug() << "撤销删除引脚:" << m_pinId;
        }
    }

    void redo() override
    {
        if (m_cellItem) {
            m_cellItem->removeConnector(m_pinId);
            qDebug() << "重做删除引脚:" << m_pinId;
        }
    }

private:
    CellItem* m_cellItem;
    QString m_pinId;
    QString m_side;
    qreal m_percentage;
    qreal m_x;
    qreal m_y;
};

#endif // COMMANDS_H
