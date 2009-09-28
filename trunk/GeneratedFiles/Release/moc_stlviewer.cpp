/****************************************************************************
** Meta object code from reading C++ file 'stlviewer.h'
**
** Created: Mon 28. Sep 15:27:26 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../stlviewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stlviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_STLViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      21,   10,   10,   10, 0x08,
      33,   10,   28,   10, 0x08,
      40,   10,   28,   10, 0x08,
      49,   10,   10,   10, 0x08,
      58,   10,   10,   10, 0x08,
      70,   10,   10,   10, 0x08,
      77,   10,   10,   10, 0x08,
      84,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_STLViewer[] = {
    "STLViewer\0\0newFile()\0open()\0bool\0"
    "save()\0saveAs()\0rotate()\0translate()\0"
    "zoom()\0home()\0about()\0"
};

const QMetaObject STLViewer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_STLViewer,
      qt_meta_data_STLViewer, 0 }
};

const QMetaObject *STLViewer::metaObject() const
{
    return &staticMetaObject;
}

void *STLViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_STLViewer))
        return static_cast<void*>(const_cast< STLViewer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int STLViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newFile(); break;
        case 1: open(); break;
        case 2: { bool _r = save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: rotate(); break;
        case 5: translate(); break;
        case 6: zoom(); break;
        case 7: home(); break;
        case 8: about(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
