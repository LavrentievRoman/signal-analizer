extern "C" {
    #include <Python.h>
}

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

#include "scoped-python-interpreter.h"

ScopedPythonInterpreter scopedPythonInterpreterInstance;

ScopedPythonInterpreter::ScopedPythonInterpreter()
{
    // Line below is needed to prevent crash when launch from Qt Creator due to error
    // "Fatal Python error: failed to get the Python codec of the filesystem encoding"
    qputenv("PYTHONHOME", "");

    Py_Initialize();
}

ScopedPythonInterpreter::~ScopedPythonInterpreter()
{
    Py_Finalize();
}
