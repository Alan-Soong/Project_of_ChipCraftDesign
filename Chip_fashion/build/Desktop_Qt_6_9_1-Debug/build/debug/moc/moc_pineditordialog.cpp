/****************************************************************************
** Meta object code from reading C++ file 'pineditordialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../pineditordialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pineditordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
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
struct qt_meta_tag_ZN15PinEditorDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto PinEditorDialog::qt_create_metaobjectdata<qt_meta_tag_ZN15PinEditorDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PinEditorDialog",
        "pinMoved",
        "",
        "PinItem*",
        "pin",
        "onAddPinClicked",
        "onRemovePinClicked",
        "onSaveToFileClicked",
        "onLoadFromFileClicked",
        "onPinSceneClicked",
        "pos",
        "onPinSelectionChanged",
        "onSideChanged",
        "index",
        "onPercentageChanged",
        "value",
        "onNameChanged",
        "text",
        "onInstanceChanged",
        "onColorChanged",
        "onSizeChanged",
        "onResizableChanged",
        "checked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'pinMoved'
        QtMocHelpers::SignalData<void(PinItem *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onAddPinClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemovePinClicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveToFileClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLoadFromFileClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPinSceneClicked'
        QtMocHelpers::SlotData<void(const QPointF &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPointF, 10 },
        }}),
        // Slot 'onPinSelectionChanged'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSideChanged'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'onPercentageChanged'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onNameChanged'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'onInstanceChanged'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'onColorChanged'
        QtMocHelpers::SlotData<void(int)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'onSizeChanged'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onResizableChanged'
        QtMocHelpers::SlotData<void(bool)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 22 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PinEditorDialog, qt_meta_tag_ZN15PinEditorDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PinEditorDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PinEditorDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PinEditorDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15PinEditorDialogE_t>.metaTypes,
    nullptr
} };

void PinEditorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PinEditorDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->pinMoved((*reinterpret_cast< std::add_pointer_t<PinItem*>>(_a[1]))); break;
        case 1: _t->onAddPinClicked(); break;
        case 2: _t->onRemovePinClicked(); break;
        case 3: _t->onSaveToFileClicked(); break;
        case 4: _t->onLoadFromFileClicked(); break;
        case 5: _t->onPinSceneClicked((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 6: _t->onPinSelectionChanged(); break;
        case 7: _t->onSideChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onPercentageChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onNameChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onInstanceChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onColorChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->onSizeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onResizableChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PinEditorDialog::*)(PinItem * )>(_a, &PinEditorDialog::pinMoved, 0))
            return;
    }
}

const QMetaObject *PinEditorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PinEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15PinEditorDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PinEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void PinEditorDialog::pinMoved(PinItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
