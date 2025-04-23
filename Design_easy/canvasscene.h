#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#pragma once

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QUndoStack>
#include "cellitem.h"

struct CellSnapshot {
    QPointF pos;
    QSizeF size;
};

class CanvasScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit CanvasScene(QObject *parent = nullptr);
    ~CanvasScene() override;

    void saveSnapshot();

    void addCellItem(CellItem *item);
    void setSelectionMode(bool enabled);
    void deleteSelectedItems();
    void undoAction();
    void redoAction();
    void zoomInButton();
    void zoomOutButton();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    bool m_leftMouseButtonPressed = false;
    bool m_selectionModeEnabled = false;
    QPoint m_lastMousePos;
    QPointF m_dragOffset; // 添加成员变量
    QGraphicsLineItem* m_tempLine; // 临时连线
    QUndoStack *undoStack;
    QVector<CellItem*> getAllCells() const;
};
#endif // CANVASSCENE_H
