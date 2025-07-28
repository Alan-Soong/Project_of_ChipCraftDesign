#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "canvasscene.h"
#include "cellitem_new.h"

class AddRectangleCommand : public QUndoCommand
{
public:
    AddRectangleCommand(CanvasScene* scene, const QPointF& pos, int& chipCounter, QUndoStack* undoStack)
        : QUndoCommand("添加矩形"), m_scene(scene), m_pos(pos), m_chipCounter(chipCounter), m_undoStack(undoStack)
    {
        m_cellItem = new CellItem();
        m_cellItem->setPos(m_pos);
        m_cellItem->setSize(QSizeF(150, 100));

        // Increment chip counter and set names
        m_chipCounter++;
        int pinCount = m_cellItem->getConnectors().size() > 0 ? m_cellItem->getConnectors().size() : 1;
        m_cellItem->setMacroName("MC" + QString::number(pinCount));
        m_cellItem->setInstanceName("C" + QString::number(m_chipCounter));
    }

    void undo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->removeItem(m_cellItem);
            m_chipCounter--; // Decrement counter on undo
            qDebug() << "撤销添加矩形 at" << m_pos << ", InstanceName: C" << m_chipCounter;
        }
    }

    void redo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->addCellItem(m_cellItem);
            qDebug() << "重做添加矩形 at" << m_pos << ", InstanceName: C" << m_chipCounter;
        }
    }

private:
    CanvasScene* m_scene;
    CellItem* m_cellItem;
    QPointF m_pos;
    int& m_chipCounter; // Reference to external counter
    QUndoStack* m_undoStack;
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
            m_cellItem->addConnector(m_side, m_percentage, 10.0, m_pinId, m_x, m_y);
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
