#ifndef CHIPMANAGER_H
#define CHIPMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include "cellitem_new.h"

/**
 * 芯片管理器 - 负责芯片的创建、命名和管理
 */
class ChipManager : public QObject
{
    Q_OBJECT

public:
    explicit ChipManager(QObject *parent = nullptr);
    ~ChipManager();

    // 芯片创建和管理
    CellItem* createNewChip(const QPointF& position = QPointF(2000, 2000), 
                           const QSizeF& size = QSizeF(150, 100));
    void updateChipNames(CellItem* cellItem);
    QString generateUniqueInstanceName();
    QString generateMacroName(int pinCount);

    // 计数器管理
    int getChipCounter() const { return m_chipCounter; }
    void incrementChipCounter() { m_chipCounter++; }
    void resetChipCounter() { m_chipCounter = 0; }

    // 芯片验证
    bool validateChipPlacement(CellItem* chip) const;
    
private:
    int m_chipCounter;
    QList<CellItem*> m_managedChips;

    void registerChip(CellItem* chip);
    void unregisterChip(CellItem* chip);
};

#endif // CHIPMANAGER_H
