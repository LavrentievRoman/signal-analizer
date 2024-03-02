extern "C" {
    #include <Python.h>
}

#include <QtCore/QDebug>

#include "py-object-ref.h"

#define ENABLE_REF_COUNT_LOG 0

PyObjectRef::PyObjectRef(PyObject *pyObjectPtr) :
    _pyObjectPtr(pyObjectPtr)
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::PyObjectRef()";

    if (_pyObjectPtr)
        qDebug() << "Initial Py_REFCNT = " << Py_REFCNT(_pyObjectPtr);
#endif
}

PyObjectRef::~PyObjectRef()
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::~PyObjectRef()";

    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT before = " << Py_REFCNT(_pyObjectPtr);
#endif

    Py_XDECREF(_pyObjectPtr);

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT after = " << Py_REFCNT(_pyObjectPtr);
#endif
}

PyObjectRef::PyObjectRef(const PyObjectRef &other)
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::PyObjectRef(const PyObjectRef &other)";
#endif

    _pyObjectPtr = other._pyObjectPtr;

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT before = " << Py_REFCNT(_pyObjectPtr);
#endif

    Py_XINCREF(_pyObjectPtr);

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT after = " << Py_REFCNT(_pyObjectPtr);
#endif
}

PyObjectRef::PyObjectRef(PyObjectRef &&other)
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::PyObjectRef(PyObjectRef &&other)";
#endif

    _pyObjectPtr = other._pyObjectPtr;
    other._pyObjectPtr = nullptr;
}

PyObjectRef &PyObjectRef::operator=(const PyObjectRef &other)
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::operator=(const PyObjectRef &other)";

    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT before = " << Py_REFCNT(_pyObjectPtr);
#endif

    Py_XDECREF(_pyObjectPtr);

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT after = " << Py_REFCNT(_pyObjectPtr);
#endif

    _pyObjectPtr = other._pyObjectPtr;

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT before = " << Py_REFCNT(_pyObjectPtr);
#endif

    Py_XINCREF(_pyObjectPtr);

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT after = " << Py_REFCNT(_pyObjectPtr);
#endif

    return *this;
}

PyObjectRef &PyObjectRef::operator=(PyObjectRef &&other)
{
#if ENABLE_REF_COUNT_LOG
    qDebug() << "PyObjectRef::operator=(PyObjectRef &&other)";

    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT before = " << Py_REFCNT(_pyObjectPtr);
#endif

    Py_XDECREF(_pyObjectPtr);

#if ENABLE_REF_COUNT_LOG
    if (_pyObjectPtr)
        qDebug() << "Py_REFCNT after = " << Py_REFCNT(_pyObjectPtr);
#endif

    _pyObjectPtr = other._pyObjectPtr;
    other._pyObjectPtr = nullptr;
    return *this;
}

PyObject *PyObjectRef::get()
{
    return _pyObjectPtr;
}
