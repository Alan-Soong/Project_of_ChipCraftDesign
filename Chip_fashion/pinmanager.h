#ifndef PINMANAGER_H
#define PINMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QGraphicsScene>
#include "pinitem.h"

class CellItem;

/**
 * 引脚管理器 - 负责引脚的创建、编辑和管理
 */
class PinManager : public QObject
{
    Q_OBJECT

public:
    // 引脚连接器结构
    struct Connector {
        QString side; // "top", "bottom", "left", "right", "custom"
        qreal percentage; // 0.0 to 100.0
        QString id;
        qreal x;
        qreal y;
        
        Connector() = default;
        Connector(const QString& s, qreal p, const QString& i, qreal xPos = 0, qreal yPos = 0)
            : side(s), percentage(p), id(i), x(xPos), y(yPos) {}
    };

    explicit PinManager(QObject *parent = nullptr);
    ~PinManager();

    // 引脚操作
    void addPin(CellItem* cellItem, const QString& side, qreal percentage, 
                qreal size, const QString& id, qreal x = 0, qreal y = 0);
    bool removePin(CellItem* cellItem, const QString& id);
    bool removePin(CellItem* cellItem, int index);
    
    // 引脚查询
    QList<Connector> getPins(const CellItem* cellItem) const;
    PinItem* findPinById(const CellItem* cellItem, const QString& id) const;
    
    // 引脚场景管理
    void updatePinScene(QGraphicsScene* scene, const CellItem* cellItem, 
                       int chipWidth, int chipHeight);
    void clearPinScene(QGraphicsScene* scene);
    
    // 引脚ID生成
    QString generatePinId(int index) const;

signals:
    void pinAdded(const QString& pinId);
    void pinRemoved(const QString& pinId);
    void pinMoved(const QString& pinId, const QPointF& newPos);

private:
    QMap<const CellItem*, QList<PinItem*>> m_cellPins;
    int m_pinCounter;
    
    PinItem* createPinItem(QGraphicsItem* parentRect, qreal size, 
                          const QString& id, qreal x, qreal y);
    void calculatePinPosition(const QString& side, qreal percentage, 
                             qreal chipWidth, qreal chipHeight, 
                             qreal pinSize, qreal& x, qreal& y);
};

Q_DECLARE_METATYPE(PinManager::Connector)

#endif // PINMANAGER_H
