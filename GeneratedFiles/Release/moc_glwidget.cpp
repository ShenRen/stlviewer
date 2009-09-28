/****************************************************************************
** Meta object code from reading C++ file 'glwidget.h'
**
** Created: Mon 28. Sep 15:27:26 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../glwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GLWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   10,    9,    9, 0x05,
      38,   10,    9,    9, 0x05,
      60,   10,    9,    9, 0x05,
      91,   82,    9,    9, 0x05,
     116,   82,    9,    9, 0x05,
     146,  141,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     165,   10,    9,    9, 0x0a,
     183,   10,    9,    9, 0x0a,
     201,   10,    9,    9, 0x0a,
     219,   82,    9,    9, 0x0a,
     240,   82,    9,    9, 0x0a,
     261,  141,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GLWidget[] = {
    "GLWidget\0\0angle\0xRotationChanged(int)\0"
    "yRotationChanged(int)\0zRotationChanged(int)\0"
    "distance\0xTranslationChanged(int)\0"
    "yTranslationChanged(int)\0zoom\0"
    "zoomChanged(float)\0setXRotation(int)\0"
    "setYRotation(int)\0setZRotation(int)\0"
    "setXTranslation(int)\0setYTranslation(int)\0"
    "setZoom(float)\0"
};

const QMetaObject GLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_GLWidget,
      qt_meta_data_GLWidget, 0 }
};

const QMetaObject *GLWidget::metaObject() const
{
    return &staticMetaObject;
}

void *GLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLWidget))
        return static_cast<void*>(const_cast< GLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int GLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: xRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: yRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: zRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: xTranslationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: yTranslationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: zoomChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: setXRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setYRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: setZRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: setXTranslation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: setYTranslation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: setZoom((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void GLWidget::xRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GLWidget::yRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GLWidget::zRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GLWidget::xTranslationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GLWidget::yTranslationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GLWidget::zoomChanged(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
