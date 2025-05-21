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
        m_cellItem = new CellItem();
        m_cellItem->setPos(m_pos);
    }

    void undo() override
    {
        m_scene->removeItem(m_cellItem);
        m_scene->update();
    }

    void redo() override
    {
        m_scene->addItem(m_cellItem);
        m_scene->update();
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
