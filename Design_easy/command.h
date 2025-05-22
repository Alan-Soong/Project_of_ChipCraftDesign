#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "canvasscene.h"
#include "cellitem.h"

class AddRectangleCommand : public QUndoCommand
{
public:

    AddRectangleCommand(CanvasScene* scene, const QPointF& pos, QUndoStack* undoStack)
        : QUndoCommand("添加矩形"), m_scene(scene), m_pos(pos), m_undoStack(undoStack)
    {
        // m_cellItem = new CellItem();
        // m_cellItem->setPos(m_pos);
        // 添加一个默认的芯片
        m_cellItem = new CellItem();
        m_cellItem->setPos(m_pos); // 使用传入的 pos
        m_cellItem->setSize(QSizeF(150, 100));
        m_cellItem->setMacroName("MC1");
        m_cellItem->setInstanceName("C1");
    }

    // void undo() override
    // {
    //     m_scene->removeItem(m_cellItem);
    //     m_scene->update();
    // }

    // void redo() override
    // {
    //     m_scene->addItem(m_cellItem);
    //     m_scene->update();
    // }
    void undo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->removeItem(m_cellItem);
            // 不删除 m_cellItem，因为 redo 需要重新添加
            qDebug() << "撤销添加矩形 at" << m_pos;
        }
    }

    void redo() override
    {
        if (m_cellItem && m_scene) {
            m_scene->addCellItem(m_cellItem);
            qDebug() << "重做添加矩形 at" << m_pos;
        }
    }

private:
    CanvasScene* m_scene;
    CellItem* m_cellItem;
    QPointF m_pos;
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
        m_scene->addItem(m_cellItem);
        m_cellItem->setPos(m_pos);
        m_scene->update();
    }

    void redo() override
    {
        m_scene->removeItem(m_cellItem);
        m_scene->update();
    }

private:
    CanvasScene* m_scene;
    CellItem* m_cellItem;
    QPointF m_pos;
    QUndoStack* m_undoStack;
};

#endif // COMMANDS_H
