// #include "portgroup.h"

// portgroup::portgroup() {}

// portgroup.cpp
#include "portgroup.h"

PortGroup::PortGroup(const QString &name,
                     const QPoint &position,
                     const QSize &size)
    : m_name(name), m_position(position), m_size(size) {}

QString PortGroup::name() const { return m_name; }
QPoint PortGroup::position() const { return m_position; }
QSize PortGroup::size() const { return m_size; }

QPoint PortGroup::connectionPoint() const {
    // Center of the rectangle: position + half size
    return QPoint(m_position.x() + m_size.width() / 2,
                  m_position.y() + m_size.height() / 2);
}

void PortGroup::setName(const QString &name) { m_name = name; }
void PortGroup::setPosition(const QPoint &pos) { m_position = pos; }
void PortGroup::setSize(const QSize &size) { m_size = size; }

QJsonObject PortGroup::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["x"] = m_position.x();
    obj["y"] = m_position.y();
    obj["width"] = m_size.width();
    obj["height"] = m_size.height();
    return obj;
}

PortGroup PortGroup::fromJson(const QJsonObject &obj) {
    QString name = obj.value("name").toString();
    int x = obj.value("x").toInt();
    int y = obj.value("y").toInt();
    int w = obj.value("width").toInt();
    int h = obj.value("height").toInt();
    return PortGroup(name, QPoint(x, y), QSize(w, h));
}
