// #ifndef UNITTEMPLATE_H
#define UNITTEMPLATE_H

// class unittemplate
// {
// public:
//     unittemplate();
// };

// #endif // UNITTEMPLATE_H

// unittemplate.h
#pragma once

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "portgroup.h"

/**
 * @brief Represents a rectangular unit template with multiple port groups.
 */
class UnitTemplate {
public:
    UnitTemplate(const QString &name = QString(), int width = 0, int height = 0);

    // Getters
    QString name() const;
    int width() const;
    int height() const;
    const QVector<PortGroup>& portGroups() const;

    // Setters
    void setName(const QString &name);
    void setWidth(int width);
    void setHeight(int height);

    // Port group management
    void addPortGroup(const PortGroup &pg);
    void insertPortGroup(int index, const PortGroup &pg);
    void removePortGroup(int index);
    void clearPortGroups();

    // Serialization to/from JSON
    QJsonObject toJson() const;
    static UnitTemplate fromJson(const QJsonObject &obj);

private:
    QString m_name;
    int m_width;
    int m_height;
    QVector<PortGroup> m_portGroups;
};

