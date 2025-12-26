/****************************************************************************
** Meta object code from reading C++ file 'cloadscenariodialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../cloadscenariodialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cloadscenariodialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CLoadScenarioDialog_t {
    QByteArrayData data[11];
    char stringdata0[244];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLoadScenarioDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLoadScenarioDialog_t qt_meta_stringdata_CLoadScenarioDialog = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CLoadScenarioDialog"
QT_MOC_LITERAL(1, 20, 26), // "on_pushButton_Load_clicked"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 28), // "on_pushButton_Cancel_clicked"
QT_MOC_LITERAL(4, 77, 28), // "on_pushButton_Browse_clicked"
QT_MOC_LITERAL(5, 106, 41), // "on_listWidget_Scenarios_itemD..."
QT_MOC_LITERAL(6, 148, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 165, 4), // "item"
QT_MOC_LITERAL(8, 170, 39), // "on_listWidget_Samples_itemDou..."
QT_MOC_LITERAL(9, 210, 27), // "on_tabWidget_currentChanged"
QT_MOC_LITERAL(10, 238, 5) // "index"

    },
    "CLoadScenarioDialog\0on_pushButton_Load_clicked\0"
    "\0on_pushButton_Cancel_clicked\0"
    "on_pushButton_Browse_clicked\0"
    "on_listWidget_Scenarios_itemDoubleClicked\0"
    "QListWidgetItem*\0item\0"
    "on_listWidget_Samples_itemDoubleClicked\0"
    "on_tabWidget_currentChanged\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLoadScenarioDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    1,   47,    2, 0x08 /* Private */,
       8,    1,   50,    2, 0x08 /* Private */,
       9,    1,   53,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void CLoadScenarioDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CLoadScenarioDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pushButton_Load_clicked(); break;
        case 1: _t->on_pushButton_Cancel_clicked(); break;
        case 2: _t->on_pushButton_Browse_clicked(); break;
        case 3: _t->on_listWidget_Scenarios_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->on_listWidget_Samples_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->on_tabWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CLoadScenarioDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLoadScenarioDialog.data,
    qt_meta_data_CLoadScenarioDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CLoadScenarioDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLoadScenarioDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLoadScenarioDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CLoadScenarioDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
