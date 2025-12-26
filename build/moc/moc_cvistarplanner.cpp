/****************************************************************************
** Meta object code from reading C++ file 'cvistarplanner.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../cvistarplanner.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cvistarplanner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CVistarPlanner_t {
    QByteArrayData data[19];
    char stringdata0[327];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CVistarPlanner_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CVistarPlanner_t qt_meta_stringdata_CVistarPlanner = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CVistarPlanner"
QT_MOC_LITERAL(1, 15, 13), // "slotMouseRead"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 9), // "mouseRead"
QT_MOC_LITERAL(4, 40, 26), // "on_pushButton_Mark_clicked"
QT_MOC_LITERAL(5, 67, 24), // "slotClearObjectSelection"
QT_MOC_LITERAL(6, 92, 32), // "on_pushButton_Initialize_clicked"
QT_MOC_LITERAL(7, 125, 27), // "on_pushButton_Start_clicked"
QT_MOC_LITERAL(8, 153, 26), // "on_pushButton_Stop_clicked"
QT_MOC_LITERAL(9, 180, 32), // "on_pushButton_ImportMaps_clicked"
QT_MOC_LITERAL(10, 213, 14), // "onLoadScenario"
QT_MOC_LITERAL(11, 228, 14), // "onSaveScenario"
QT_MOC_LITERAL(12, 243, 16), // "onScenarioLoaded"
QT_MOC_LITERAL(13, 260, 8), // "Scenario"
QT_MOC_LITERAL(14, 269, 8), // "scenario"
QT_MOC_LITERAL(15, 278, 15), // "onScenarioSaved"
QT_MOC_LITERAL(16, 294, 8), // "filePath"
QT_MOC_LITERAL(17, 303, 15), // "onScenarioError"
QT_MOC_LITERAL(18, 319, 7) // "message"

    },
    "CVistarPlanner\0slotMouseRead\0\0mouseRead\0"
    "on_pushButton_Mark_clicked\0"
    "slotClearObjectSelection\0"
    "on_pushButton_Initialize_clicked\0"
    "on_pushButton_Start_clicked\0"
    "on_pushButton_Stop_clicked\0"
    "on_pushButton_ImportMaps_clicked\0"
    "onLoadScenario\0onSaveScenario\0"
    "onScenarioLoaded\0Scenario\0scenario\0"
    "onScenarioSaved\0filePath\0onScenarioError\0"
    "message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CVistarPlanner[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x08 /* Private */,
       4,    0,   77,    2, 0x08 /* Private */,
       5,    0,   78,    2, 0x08 /* Private */,
       6,    0,   79,    2, 0x08 /* Private */,
       7,    0,   80,    2, 0x08 /* Private */,
       8,    0,   81,    2, 0x08 /* Private */,
       9,    0,   82,    2, 0x08 /* Private */,
      10,    0,   83,    2, 0x08 /* Private */,
      11,    0,   84,    2, 0x08 /* Private */,
      12,    1,   85,    2, 0x08 /* Private */,
      15,    1,   88,    2, 0x08 /* Private */,
      17,    1,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   18,

       0        // eod
};

void CVistarPlanner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CVistarPlanner *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotMouseRead((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_pushButton_Mark_clicked(); break;
        case 2: _t->slotClearObjectSelection(); break;
        case 3: _t->on_pushButton_Initialize_clicked(); break;
        case 4: _t->on_pushButton_Start_clicked(); break;
        case 5: _t->on_pushButton_Stop_clicked(); break;
        case 6: _t->on_pushButton_ImportMaps_clicked(); break;
        case 7: _t->onLoadScenario(); break;
        case 8: _t->onSaveScenario(); break;
        case 9: _t->onScenarioLoaded((*reinterpret_cast< const Scenario(*)>(_a[1]))); break;
        case 10: _t->onScenarioSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->onScenarioError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CVistarPlanner::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CVistarPlanner.data,
    qt_meta_data_CVistarPlanner,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CVistarPlanner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CVistarPlanner::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CVistarPlanner.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int CVistarPlanner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
