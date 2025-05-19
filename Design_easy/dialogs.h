#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsScene>

// 前向声明 CellItem 和 Connector
class CellItem;
namespace CellItemNS {

struct Connector {
    QString side; // "top", "bottom", "left", "right"
    qreal percentage; // 0.0 to 100.0
    QString id;
    qreal x;
    qreal y;
    Connector() = default;
    Connector(const QString& s, qreal p, const QString& i, qreal xPos = 0, qreal yPos = 0)
        : side(s), percentage(p), id(i), x(xPos), y(yPos) {}
};
}
Q_DECLARE_METATYPE(CellItemNS::Connector)

namespace Ui {
class Dialogs;
}

// 信息结构体，可扩展为大小、接口数量等
struct ComponentInfo {
    QString name;
    int width = 0;
    int height = 0;
    int portCount = 0;
    QList<CellItemNS::Connector> pins; // 使用 CellItem::Connector
    QString color;
    // QList<PinInfo> pins;   // 引脚列表
};

// 在 dialogs.h 中声明
class PinItem : public QGraphicsEllipseItem {
public:
    PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent = nullptr);
    void updateConnector(const QString& id, qreal x, qreal y);
    QString getSide() const { return m_side; }
    qreal getPercentage() const { return m_percentage; }
    QString getId() const { return m_id; }
    qreal getX() const { return m_x; }
    qreal getY() const { return m_y; }
    void setParentRect(QGraphicsItem* parentRect) { this->parentRect = parentRect; } // 新增 setter
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    QGraphicsItem* parentRect; // 父矩形，用于边界检查
    QString m_side;
    qreal m_percentage;
    QString m_id;
    qreal m_x;
    qreal m_y;
    QPointF restrictToEdge(const QPointF& pos, qreal width, qreal height, QString& side, qreal& percentage) const;
};


class Dialogs : public QDialog
{
    Q_OBJECT

public:
    explicit Dialogs(QGraphicsItem* item, QWidget *parent = nullptr);
    ComponentInfo currentInfo;
    void on_pinMoved(PinItem* pin); // 处理引脚移动
    // void accept() override;
    // void reject() override;
    ~Dialogs();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void settingsSaved(const ComponentInfo& info);
    void pinMoved(PinItem* pin); // 新信号，通知引脚移动

private slots:
    void saveSetting();

    void on_nameEdit_textEdited(const QString &arg1);

    void on_saveButton_clicked();

    void on_colorCombo_activated(int index);

    void on_addPinButton_clicked(); // 新增：添加引脚的槽

    void on_pinSceneClicked(const QPointF& pos);

private:
    QGraphicsItem* targetItem = nullptr;
    Ui::Dialogs *ui;
    QLineEdit* nameEdit;
    QComboBox* colorCombo;
    QCheckBox* enableResize;
    QGraphicsRectItem* RectItem = nullptr;
    QGraphicsRectItem* chipRect;

    QList<PinItem*> pinItems; // 存储引脚的图形项
    QGraphicsScene* pinScene;
    bool m_addingPin = false; // 新增：跟踪添加引脚模式

    void addPin(const QString& side, qreal percentage, qreal size, const QString& id);
    void updatePins();
    void saveToFile(const QString& defaultName); // 声明
    void loadFromFile(const QString& defaultName); // 声明
    void setupPinScene();
    void updatePinScene();
};


#endif // DIALOGS_H
