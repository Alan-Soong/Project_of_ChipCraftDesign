#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "canvasscene.h"
#include "cellitem_new.h"
#include "chipmanager.h"

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
