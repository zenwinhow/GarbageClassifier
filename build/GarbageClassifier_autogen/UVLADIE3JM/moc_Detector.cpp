/****************************************************************************
** Meta object code from reading C++ file 'Detector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Detector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Detector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Detector_t {
    QByteArrayData data[10];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Detector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Detector_t qt_meta_stringdata_Detector = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Detector"
QT_MOC_LITERAL(1, 9, 9), // "detection"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 5), // "frame"
QT_MOC_LITERAL(4, 26, 21), // "std::vector<cv::Rect>"
QT_MOC_LITERAL(5, 48, 5), // "boxes"
QT_MOC_LITERAL(6, 54, 18), // "std::vector<float>"
QT_MOC_LITERAL(7, 73, 5), // "confs"
QT_MOC_LITERAL(8, 79, 24), // "std::vector<std::string>"
QT_MOC_LITERAL(9, 104, 6) // "labels"

    },
    "Detector\0detection\0\0frame\0"
    "std::vector<cv::Rect>\0boxes\0"
    "std::vector<float>\0confs\0"
    "std::vector<std::string>\0labels"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Detector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QImage, 0x80000000 | 4, 0x80000000 | 6, 0x80000000 | 8,    3,    5,    7,    9,

       0        // eod
};

void Detector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Detector *_t = static_cast<Detector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->detection((*reinterpret_cast< const QImage(*)>(_a[1])),(*reinterpret_cast< const std::vector<cv::Rect>(*)>(_a[2])),(*reinterpret_cast< const std::vector<float>(*)>(_a[3])),(*reinterpret_cast< const std::vector<std::string>(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Detector::*_t)(const QImage & , const std::vector<cv::Rect> & , const std::vector<float> & , const std::vector<std::string> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Detector::detection)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Detector::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Detector.data,
      qt_meta_data_Detector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Detector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Detector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Detector.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int Detector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Detector::detection(const QImage & _t1, const std::vector<cv::Rect> & _t2, const std::vector<float> & _t3, const std::vector<std::string> & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
