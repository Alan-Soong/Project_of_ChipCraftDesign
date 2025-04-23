// #ifndef PORTGROUP_H
#define PORTGROUP_H

// class portgroup
// {
// public:
//     portgroup();
// };

// #endif // PORTGROUP_H


// #include "portgroup.h"

// portgroup::portgroup() {}

// portgroup.h
#pragma once

#include <QString>
#include <QPoint>
#include <QSize>
#include <QJsonObject>

/**
 * @brief Represents a rectangular port group within a unit template.
 */
class PortGroup {
public:
    PortGroup(const QString &name = QString(),
              const QPoint &position = QPoint(),
              const QSize &size = QSize());

    // Getters
    QString name() const;
    QPoint position() const;
    QSize size() const;
    /**
     * @brief Returns the center point of the port (connection point).
     */
    QPoint connectionPoint() const;

    // Setters
    void setName(const QString &name);
    void setPosition(const QPoint &pos);
    void setSize(const QSize &size);

    // Serialization to/from JSON
    QJsonObject toJson() const;
    static PortGroup fromJson(const QJsonObject &obj);

private:
    QString m_name;
    QPoint m_position;
    QSize m_size;
};
