// #ifndef CUSTOMSCENE_H
// #define CUSTOMSCENE_H

// class customscene
// {
// public:
//     customscene();
// };

// #endif // CUSTOMSCENE_H

// customscene.h
#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include "unittemplate.h"

class CustomScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit CustomScene(QObject *parent = nullptr);

    /**
     * @brief Load and display a unit template on the scene.
     */
    void loadUnitTemplate(const UnitTemplate &unit);
    void loadUnit(const UnitTemplate &unit);  // ✅ 添加声明

signals:
    /**
     * @brief Emitted when the scene is clicked, reporting the position.
     */
    void sceneClicked(const QPoint &position);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void clearScene();
};
