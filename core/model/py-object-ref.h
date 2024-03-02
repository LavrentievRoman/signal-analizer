#pragma once

extern "C" typedef struct _object PyObject;

class PyObjectRef
{
public:

    PyObjectRef(PyObject *pyObjectPtr = nullptr);
    ~PyObjectRef();

    PyObjectRef(const PyObjectRef &other);
    PyObjectRef(PyObjectRef &&other);
    PyObjectRef &operator=(const PyObjectRef &other);
    PyObjectRef &operator=(PyObjectRef &&other);

    PyObject *get();

private:

    PyObject *_pyObjectPtr;
};
