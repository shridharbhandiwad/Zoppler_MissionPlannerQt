/****************************************************************************
** Meta object code from reading C++ file 'cmaptoolselectobjects.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../MapDisplay/cmaptoolselectobjects.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cmaptoolselectobjects.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CMapToolSelectObjects_t {
    QByteArrayData data[7];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMapToolSelectObjects_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMapToolSelectObjects_t qt_meta_stringdata_CMapToolSelectObjects = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CMapToolSelectObjects"
QT_MOC_LITERAL(1, 22, 13), // "pointSelected"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 10), // "QgsPointXY"
QT_MOC_LITERAL(4, 48, 5), // "point"
QT_MOC_LITERAL(5, 54, 14), // "objectSelected"
QT_MOC_LITERAL(6, 69, 8) // "objectId"

    },
    "CMapToolSelectObjects\0pointSelected\0"
    "\0QgsPointXY\0point\0objectSelected\0"
    "objectId"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMapToolSelectObjects[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       5,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

void CMapToolSelectObjects::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CMapToolSelectObjects *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pointSelected((*reinterpret_cast< const QgsPointXY(*)>(_a[1]))); break;
        case 1: _t->objectSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QgsPointXY >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CMapToolSelectObjects::*)(const QgsPointXY & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CMapToolSelectObjects::pointSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CMapToolSelectObjects::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CMapToolSelectObjects::objectSelected)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CMapToolSelectObjects::staticMetaObject = { {
    QMetaObject::SuperData::link<QgsMapTool::staticMetaObject>(),
    qt_meta_stringdata_CMapToolSelectObjects.data,
    qt_meta_data_CMapToolSelectObjects,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CMapToolSelectObjects::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMapToolSelectObjects::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CMapToolSelectObjects.stringdata0))
        return static_cast<void*>(this);
    return QgsMapTool::qt_metacast(_clname);
}

int CMapToolSelectObjects::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QgsMapTool::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CMapToolSelectObjects::pointSelected(const QgsPointXY & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CMapToolSelectObjects::objectSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
