#ifndef PINEDITORDIALOG_H
#define PINEDITORDIALOG_H

#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QUndoStack>

QT_BEGIN_NAMESPACE
namespace Ui { class PinEditorDialog; }
QT_END_NAMESPACE

class CellItem;
class PinManager;
class PinItem;

/**
 * 引脚编辑对话框 - 专门负责引脚的添加、编辑和管理
 */
class PinEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PinEditorDialog(CellItem* item, QUndoStack* undoStack = nullptr, QWidget *parent = nullptr);
    ~PinEditorDialog();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void reject() override; // 重写reject方法，用于取消时恢复原始尺寸

signals:
    void pinMoved(PinItem* pin);

private slots:
    void onAddPinClicked();
    void onRemovePinClicked();
    void onSaveToFileClicked();
    void onLoadFromFileClicked();
    void onPinSceneClicked(const QPointF& pos);
    void onPinSelectionChanged();
    void onSideChanged(int index);
    void onPercentageChanged(int value);

    // 芯片属性变化槽函数
    void onNameChanged(const QString& text);
    void onInstanceChanged(const QString& text);
    void onColorChanged(int index);
    void onSizeChanged(int value);
    void onResizableChanged(bool checked);

private:
    void setupConnections();
    void loadChipProperties();
    void setupPinScene();
    void updatePinScene();
    void updatePinList();
    void updatePinProperties();
    void enterAddPinMode();
    void exitAddPinMode();
    void updateChipColor();
    QColor getColorFromString(const QString& colorName);
    QString getCurrentColorName();
    bool pinIdExists(const QString& id);  // 检查引脚ID是否已存在

    Ui::PinEditorDialog *ui;
    CellItem* m_targetItem;
    PinManager* m_pinManager;
    QUndoStack* m_undoStack;  // 添加撤销栈引用

    // 场景和图形组件
    QGraphicsScene* m_pinScene;
    QGraphicsRectItem* m_chipRect;

    // 状态变量
    bool m_addingPin;
    PinItem* m_selectedPin;
    QList<PinItem*> m_pinItems;
    QString m_currentColor;  // 当前芯片颜色
    QSizeF m_originalSize;   // 保存原始尺寸，用于取消时恢复
};

#endif // PINEDITORDIALOG_H
