/****************************************************************************
** Meta object code from reading C++ file 'hh.h'
**
** Created: Thu Dec 20 18:14:20 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QtDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hh.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtListDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QtListDialog[] = {
	"QtListDialog\0\0OnOpenFile()\0\0OnSaveAs()\0"
};

const QMetaObject QtListDialog::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtListDialog,
      qt_meta_data_QtListDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtListDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtListDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtListDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtListDialog))
        return static_cast<void*>(const_cast< QtListDialog*>(this));
    if (!strcmp(_clname, "Ui::MainWindow"))
        return static_cast< Ui::MainWindow*>(const_cast< QtListDialog*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QtListDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnOpenFile(); break;
		case 1: OnSaveAs(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
