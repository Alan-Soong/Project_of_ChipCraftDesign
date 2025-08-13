#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "cellitem_new.h"

class CanvasScene;

/**
 * 文件管理器 - 负责设计文件的读取、保存和导出
 */
class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();

    // 文件操作
    bool openDesignFile(const QString& filePath, CanvasScene* scene);
    bool saveDesignFile(const QString& filePath, const QList<CellItem*>& cellItems, CanvasScene* scene);
    bool exportMacroFile(const QString& filePath, const QList<CellItem*>& cellItems);

    // 组件文件操作
    bool saveComponentToFile(const QString& filePath, const CellItem* cellItem);
    bool loadComponentFromFile(const QString& filePath, CellItem* cellItem);

    // 工具方法
    QString getLastError() const { return m_lastError; }
    void clearError() { m_lastError.clear(); }

signals:
    void fileOperationCompleted(const QString& operation, bool success);
    void progressUpdate(int percentage);

private:
    QString m_lastError;
    int m_chipCounter; // 用于文件读取时的芯片计数

    // 解析方法
    bool parseDieSize(const QString& line, CanvasScene* scene);
    bool parseMacroDefinition(const QString& line, QTextStream& stream,
                              QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes,
                              CanvasScene* scene);
    bool parseInstance(const QString& line, CanvasScene* scene,
                       const QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes,
                       QMap<QString, CellItem*>& cellMap,
                       QMap<QString, QString>& instanceNameMapping);
    bool parseNet(const QString& line, QTextStream& stream, CanvasScene* scene,
                  const QMap<QString, CellItem*>& cellMap,
                  const QMap<QString, QString>& instanceNameMapping);

    // 生成方法
    bool generateMacroDefinitions(QTextStream& stream, const QList<CellItem*>& cellItems, CanvasScene* scene);
    bool generateInstances(QTextStream& stream, const QList<CellItem*>& cellItems, CanvasScene* scene);
    bool generateNets(QTextStream& stream, const QList<CellItem*>& cellItems);

    // 单位转换方法
    qreal convertFromFileUnit(qreal value, const QString& canvasUnit) const;
    qreal convertToFileUnit(qreal value, const QString& canvasUnit) const;

    void setError(const QString& error);
};

#endif // FILEMANAGER_H
