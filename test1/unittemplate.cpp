// #include "unittemplate.h"

// unittemplate::unittemplate() {}

// unittemplate.cpp
#include "unittemplate.h"
#include <QtGlobal>

UnitTemplate::UnitTemplate(const QString &name, int width, int height)
    : m_name(name), m_width(width), m_height(height) {}

QString UnitTemplate::name() const { return m_name; }
int UnitTemplate::width() const { return m_width; }
int UnitTemplate::height() const { return m_height; }

const QVector<PortGroup>& UnitTemplate::portGroups() const { return m_portGroups; }

void UnitTemplate::setName(const QString &name) { m_name = name; }
void UnitTemplate::setWidth(int width) { m_width = width; }
void UnitTemplate::setHeight(int height) { m_height = height; }

void UnitTemplate::addPortGroup(const PortGroup &pg) {
    m_portGroups.append(pg);
}

void UnitTemplate::insertPortGroup(int index, const PortGroup &pg) {
    if (index < 0 || index > m_portGroups.size()) {
        addPortGroup(pg);
    } else {
        m_portGroups.insert(index, pg);
    }
}

void UnitTemplate::removePortGroup(int index) {
    if (index >= 0 && index < m_portGroups.size()) {
        m_portGroups.removeAt(index);
    }
}

void UnitTemplate::clearPortGroups() {
    m_portGroups.clear();
}

QJsonObject UnitTemplate::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["width"] = m_width;
    obj["height"] = m_height;
    QJsonArray arr;
    for (const auto &pg : m_portGroups) {
        arr.append(pg.toJson());
    }
    obj["portGroups"] = arr;
    return obj;
}

UnitTemplate UnitTemplate::fromJson(const QJsonObject &obj) {
    QString name = obj.value("name").toString();
    int w = obj.value("width").toInt();
    int h = obj.value("height").toInt();
    UnitTemplate ut(name, w, h);
    QJsonArray arr = obj.value("portGroups").toArray();
    for (const auto &v : arr) {
        ut.addPortGroup(PortGroup::fromJson(v.toObject()));
    }
    return ut;
}
