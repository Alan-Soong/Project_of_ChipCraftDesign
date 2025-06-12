/****************************************************************************
** Meta object code from reading C++ file 'dialogs.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dialogs.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialogs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN7DialogsE_t {};
} // unnamed namespace

template <> constexpr inline auto Dialogs::qt_create_metaobjectdata<qt_meta_tag_ZN7DialogsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Dialogs",
        "settingsSaved",
        "",
        "ComponentInfo",
        "info",
        "pinMoved",
        "PinItem*",
        "pin",
        "saveSetting",
        "on_nameEdit_textEdited",
        "arg1",
        "on_saveButton_clicked",
        "on_colorCombo_activated",
        "index",
        "on_addPinButton_clicked",
        "on_pinSceneClicked",
        "pos",
        "on_removePinButton_clicked",
        "on_sideCombo_currentIndexChanged",
        "on_xSpinBox_valueChanged",
        "value",
        "on_ySpinBox_valueChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'settingsSaved'
        QtMocHelpers::SignalData<void(const ComponentInfo &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'pinMoved'
        QtMocHelpers::SignalData<void(PinItem *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'saveSetting'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_nameEdit_textEdited'
        QtMocHelpers::SlotData<void(const QString &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'on_saveButton_clicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_colorCombo_activated'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_addPinButton_clicked'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_pinSceneClicked'
        QtMocHelpers::SlotData<void(const QPointF &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPointF, 16 },
        }}),
        // Slot 'on_removePinButton_clicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_sideCombo_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_xSpinBox_valueChanged'
        QtMocHelpers::SlotData<void(double)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 20 },
        }}),
        // Slot 'on_ySpinBox_valueChanged'
        QtMocHelpers::SlotData<void(double)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 20 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Dialogs, qt_meta_tag_ZN7DialogsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Dialogs::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7DialogsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7DialogsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7DialogsE_t>.metaTypes,
    nullptr
} };

void Dialogs::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Dialogs *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->settingsSaved((*reinterpret_cast< std::add_pointer_t<ComponentInfo>>(_a[1]))); break;
        case 1: _t->pinMoved((*reinterpret_cast< std::add_pointer_t<PinItem*>>(_a[1]))); break;
        case 2: _t->saveSetting(); break;
        case 3: _t->on_nameEdit_textEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->on_saveButton_clicked(); break;
        case 5: _t->on_colorCombo_activated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->on_addPinButton_clicked(); break;
        case 7: _t->on_pinSceneClicked((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 8: _t->on_removePinButton_clicked(); break;
        case 9: _t->on_sideCombo_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->on_xSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 11: _t->on_ySpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Dialogs::*)(const ComponentInfo & )>(_a, &Dialogs::settingsSaved, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Dialogs::*)(PinItem * )>(_a, &Dialogs::pinMoved, 1))
            return;
    }
}

const QMetaObject *Dialogs::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dialogs::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7DialogsE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Dialogs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Dialogs::settingsSaved(const ComponentInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Dialogs::pinMoved(PinItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
