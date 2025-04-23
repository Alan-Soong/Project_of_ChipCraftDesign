// #include "customscene.h"

// customscene::customscene() {}

// customscene.cpp
#include "customscene.h"
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

CustomScene::CustomScene(QObject *parent)
    : QGraphicsScene(parent) {}

void CustomScene::loadUnitTemplate(const UnitTemplate &unit) {
    clearScene();

    // Draw the unit body (rectangle)
    QGraphicsRectItem *unitRect = addRect(0, 0, unit.width(), unit.height(), QPen(Qt::black), QBrush(Qt::lightGray));

    // Draw the name of the unit on top
    QGraphicsTextItem *label = addText(unit.name());
    label->setPos(0, -20);

    // Draw all port groups
    for (const PortGroup &pg : unit.portGroups()) {
        QRect pgRect(pg.position(), pg.size());
        QGraphicsRectItem *portItem = addRect(pgRect, QPen(Qt::blue), QBrush(Qt::cyan));

        // Port name label
        QGraphicsTextItem *pgLabel = addText(pg.name());
        pgLabel->setPos(pg.position().x(), pg.position().y() - 15);
    }
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPoint pos = event->scenePos().toPoint();
    emit sceneClicked(pos);
    QGraphicsScene::mousePressEvent(event);
}

void CustomScene::clearScene() {
    this->clear();
}

void CustomScene::loadUnit(const UnitTemplate &unit){

}
