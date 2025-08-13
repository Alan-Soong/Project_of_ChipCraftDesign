#include "chipmanager.h"
#include <QDebug>

ChipManager::ChipManager(QObject *parent)
    : QObject(parent)
    , m_chipCounter(0)
{
}

ChipManager::~ChipManager()
{
    m_managedChips.clear();
}

CellItem* ChipManager::createNewChip(const QPointF& position, const QSizeF& size)
{
    incrementChipCounter();

    CellItem* chip = new CellItem();
    chip->setPos(position);
    chip->setSize(size);

    // 新芯片默认没有引脚，宏名称为MC0
    // 不添加默认引脚，让用户通过对话框手动添加

    // 根据引脚数量设置宏名称（0个引脚对应MC0）
    int pinCount = chip->getConnectors().size(); // 应该是0
    chip->setMacroName(generateMacroName(pinCount));

    // 根据芯片计数器设置实例名称
    chip->setInstanceName(generateUniqueInstanceName());

    registerChip(chip);

    qDebug() << "Created new chip with MacroName:" << chip->getMacroName()
             << ", InstanceName:" << chip->getInstanceName()
             << ", PinCount:" << pinCount;

    return chip;
}

void ChipManager::updateChipNames(CellItem* cellItem)
{
    if (!cellItem) return;

    // 更新宏名称基于引脚数量
    int pinCount = cellItem->getConnectors().size();
    // 引脚数量可以为0，对应MC0
    cellItem->setMacroName(generateMacroName(pinCount));

    qDebug() << "Updated chip names: MacroName=" << cellItem->getMacroName()
             << ", InstanceName=" << cellItem->getInstanceName()
             << ", PinCount=" << pinCount;
}

QString ChipManager::generateUniqueInstanceName()
{
    return "C" + QString::number(m_chipCounter);
}

QString ChipManager::generateMacroName(int pinCount)
{
    return "MC" + QString::number(pinCount);
}

bool ChipManager::validateChipPlacement(CellItem* chip) const
{
    if (!chip) return false;

    // 检查芯片是否与其他芯片重叠
    for (const CellItem* existingChip : m_managedChips) {
        if (existingChip != chip && chip->isOverlapping(existingChip)) {
            return false;
        }
    }

    return true;
}

void ChipManager::registerChip(CellItem* chip)
{
    if (chip && !m_managedChips.contains(chip)) {
        m_managedChips.append(chip);
    }
}

void ChipManager::unregisterChip(CellItem* chip)
{
    m_managedChips.removeAll(chip);
}
