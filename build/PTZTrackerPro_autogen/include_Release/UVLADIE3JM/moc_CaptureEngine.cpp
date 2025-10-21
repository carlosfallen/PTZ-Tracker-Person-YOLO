/****************************************************************************
** Meta object code from reading C++ file 'CaptureEngine.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/CaptureEngine.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CaptureEngine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
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
struct qt_meta_tag_ZN13CaptureEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto CaptureEngine::qt_create_metaobjectdata<qt_meta_tag_ZN13CaptureEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CaptureEngine",
        "frameReady",
        "",
        "frame",
        "fpsUpdated",
        "fps",
        "detectionCount",
        "count"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'frameReady'
        QtMocHelpers::SignalData<void(const QImage &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QImage, 3 },
        }}),
        // Signal 'fpsUpdated'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Signal 'detectionCount'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CaptureEngine, qt_meta_tag_ZN13CaptureEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CaptureEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CaptureEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CaptureEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13CaptureEngineE_t>.metaTypes,
    nullptr
} };

void CaptureEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CaptureEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->frameReady((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 1: _t->fpsUpdated((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->detectionCount((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CaptureEngine::*)(const QImage & )>(_a, &CaptureEngine::frameReady, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CaptureEngine::*)(double )>(_a, &CaptureEngine::fpsUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CaptureEngine::*)(int )>(_a, &CaptureEngine::detectionCount, 2))
            return;
    }
}

const QMetaObject *CaptureEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CaptureEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CaptureEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CaptureEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CaptureEngine::frameReady(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CaptureEngine::fpsUpdated(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void CaptureEngine::detectionCount(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
