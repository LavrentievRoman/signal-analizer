extern "C" {
    #include <Python.h>
}

#include "analyzer.h"

#include <QtCore/QDebug>
#include <QtCore/QJsonObject>

#include <QtCore/QCoreApplication>

#define ENABLE_REDIRECTION_PYTHON_STDERR_TO_STRING

#define LOG_ERROR_MESSAGE(message) qDebug().nospace().noquote() << __CLASS__ << "::" << __FUNCTION__ << "(): Error: " << message

#define __CLASS__ "Analyzer"
bool Analyzer::_wasFitProgressCallbackCalled = false;
AnalyzerObserver Analyzer::Observer = AnalyzerObserver();

Analyzer::Analyzer(std::shared_ptr<AnalyzerInstanceConfig> config)
    : _config(config)
    , _analyzerInputSize(_config->inputSize())
    , _isAnalyzerLoaded(false)
    , _state(AnalyzerState::NotTrained)
    , _errorMessagesBuffer("")
{}

Analyzer::~Analyzer()
{
}

AnalyzerState Analyzer::state() const
{
    return _state;
}

bool Analyzer::isLoaded() const
{
    return _isAnalyzerLoaded;
}

bool Analyzer::load()
{
    if (_isAnalyzerLoaded) {
        return true;
    }

    setState(AnalyzerState::Loading);

    if (!_loadRequiredPythonModules()) {
        return false;
    }

#if defined(ENABLE_REDIRECTION_PYTHON_STDERR_TO_STRING)
    if (!_redirectPythonStdErrToString()) {
        return false;
    }
#endif

    if (!_addModuleFolderToSysPath()) {
        return false;
    }

    if (!_createPythonAnalyzerObject()) {
        return false;
    }

    if (!_initializeAnalyzerCallbacks()) {
        return false;
    }

    _isAnalyzerLoaded = true;
    setState(AnalyzerState::NotTrained);
    return true;
}

void Analyzer::unload()
{
    if (!_isAnalyzerLoaded) {
        return;
    }

    // WARNING: [tvz] need to test
    _pythonModules.clear();
    _analyzerPythonModule = nullptr;
    _analyzerPythonClassInstance = nullptr;
    _onFitProgressMethodDef = nullptr;

    _isAnalyzerLoaded = false;
}

void Analyzer::fit(const QVector<double> &trainData)
{
    if (!_isAnalyzerLoaded) {
        onError(QObject::tr("Ошибка: не удалось загрузить выбранный анализатор."));
        return;
    }

    if (!_analyzerPythonClassInstance.get()) {
        onError(QObject::tr("Ошибка: отсутствует требуемый экземпляр класса в модуле анализатора."));
        return;
    }

    setState(AnalyzerState::Training);

    _wasFitProgressCallbackCalled = false;

    Py_ssize_t trainDataSize = trainData.size();
    PyObjectRef pyTrainDataList = PyList_New(trainDataSize);

    if (!pyTrainDataList.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось создать список данных для обучения:"));
        setState(AnalyzerState::FailTrained);
        return;
    }

    for (Py_ssize_t index = 0; index < trainDataSize; ++index) {
        PyObject *trainDataObj = PyFloat_FromDouble(trainData[index]);

        if (!trainDataObj) {
            _writePythonErrorMessages();
            onError(QObject::tr("Произошла ошибка при конвертации данных для обучения:"));
        }

        // When we pass PyObject* to PyList_SetItem function, it steals reference and takes ownership,
        // so we don't need to decrement counter on passed PyObject*
        (void)PyList_SetItem(pyTrainDataList.get(), index, trainDataObj);
    }

    PyObjectRef pyTrainResult = PyObject_CallMethod(
        _analyzerPythonClassInstance.get(),
        _fitMethodName,
        "O",
        pyTrainDataList.get()
    );

    if (!pyTrainResult.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Произошла ошибка при обучении модели:"));
        setState(AnalyzerState::FailTrained);
    }

    if (_state != AnalyzerState::FailTrained) {
        setState(AnalyzerState::SuccessTrained);
    }
}

double Analyzer::estimate(const QVector<double> &inputData)
{
    if (!_isAnalyzerLoaded) {
        onError(QObject::tr("Ошибка: не удалось загрузить выбранный анализатор."));
        return 0.0;
    }

    if (!_analyzerPythonClassInstance.get()) {
        onError(QObject::tr("Ошибка: отсутствует требуемый экземпляр класса в модуле анализатора."));
        return 0.0;
    }

    double estimationResult = 0.0;

    // Probably we should check here input vector size and compare with model input size.
#if 0
    Q_ASSERT(inputData.size() == _analyzerInputSize);
#endif

    Py_ssize_t inputDataSize = inputData.size();
    PyObjectRef pyInputDataList = PyList_New(inputDataSize);

    if (!pyInputDataList.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось создать список данных для анализа:"));
        return 0.0;
    }

    for (Py_ssize_t index = 0; index < inputDataSize; ++index) {
        PyObject *inputDataObj = PyFloat_FromDouble(inputData[index]);

        if (!inputDataObj) {
            _writePythonErrorMessages();
            onError(QObject::tr("Произошла ошибка при конвертации данных для анализа:"));
        }

        // When we pass PyObject* to PyList_SetItem function, it steals reference and takes ownership,
        // so we don't need to decrement counter on passed PyObject*
        (void)PyList_SetItem(pyInputDataList.get(), index, inputDataObj);
    }

    PyObjectRef pyEstimationResult = PyObject_CallMethod(
        _analyzerPythonClassInstance.get(),
        _estimateMethodName,
        "O",
        pyInputDataList.get()
    );

    if (pyEstimationResult.get()) {
        if (PyFloat_Check(pyEstimationResult.get())) {
            estimationResult = PyFloat_AsDouble(pyEstimationResult.get());
        }
        else if (int result = PyObject_IsTrue(pyEstimationResult.get()); result != -1) {
            estimationResult = (result == 1) ? 1.0 : 0.0;
        }
        else {
            _addMessageToBuffer(QObject::tr("Результат анализа не является дробным числом или "
                                            "не может быть сконвертирован к логическому типу."));
            _writePythonErrorMessages();
            onError(QObject::tr("Произошла ошибка при анализе данных моделью:"));
        }
    }
    else {
        _writePythonErrorMessages();
        onError(QObject::tr("Произошла ошибка при анализе данных моделью:"));
    }

    return estimationResult;
}

void Analyzer::onError(QString message)
{
    if (!_errorMessagesBuffer.isEmpty()) {
        message += "\r\n" + _errorMessagesBuffer;
        _errorMessagesBuffer.clear();
    }
    emit Observer.error(message);
}

void Analyzer::setState(AnalyzerState state)
{
    if (_state != state) {
        _state = state;
        emit Observer.stateChanged(_state);
        QCoreApplication::processEvents(); // FIXME: [tvz] remove after threading implement
    }
}

PyObject *Analyzer::_loadPythonModule(const QString &moduleName)
{
    PyObject *pythonModule = PyImport_ImportModule(moduleName.toUtf8().data());
    if (!pythonModule) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось загрузить модуль Python \"%1\".").arg(moduleName));
    }
    return pythonModule;
}

bool Analyzer::_loadRequiredPythonModules()
{
    for (const QString &moduleName : _requiredPythonModules) {
        QCoreApplication::processEvents(); // FIXME: [tvz] remove after threading implement
        PyObjectRef pythonModule = _loadPythonModule(moduleName);
        if (pythonModule.get()) {
            _pythonModules.insert(moduleName, pythonModule);
        }
        else {
            return false;
        }
    }

    return true;
}

bool Analyzer::_redirectPythonStdErrToString()
{
    PyObject *pythonSysModule = _pythonModules["sys"].get();
    PyObject *pythonIoModule = _pythonModules["io"].get();

    Q_ASSERT(pythonSysModule != nullptr);
    Q_ASSERT(pythonIoModule != nullptr);

    PyObjectRef pythonStringIoClass = PyObject_GetAttrString(pythonIoModule, "StringIO");
    if (!pythonStringIoClass.get()) {
        onError(QObject::tr("Ошибка: не удалось загрузить класс \"StringIO\" из модуля \"io\" Python."));
        return false;
    }

    PyObjectRef pythonStringIoClassInstance = PyObject_CallObject(pythonStringIoClass.get(), nullptr);
    if (!pythonStringIoClassInstance.get()) {
        onError(QObject::tr("Ошибка: не удалось создать экземпляр Python класса \"StringIO\"."));
        return false;
    }

    if (PyObject_SetAttrString(pythonSysModule, "stderr", pythonStringIoClassInstance.get()) != 0) {
        onError(QObject::tr("Ошибка: не удалось установить значение Python переменной \"sys.stderr\"."));
        return false;
    }

    return true;
}

bool Analyzer::_addModuleFolderToSysPath()
{
    PyObject *pythonSysModule = _pythonModules["sys"].get();

    Q_ASSERT(pythonSysModule != nullptr);

    PyObjectRef pythonSysPath = PyObject_GetAttrString(pythonSysModule, "path");
    if (!pythonSysPath.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось получить значение Python переменной \"sys.path\"."));
        return false;
    }

    QString folderPath = AnalyzerClassConfig::RootFolderPath + _config->classConfig().folderName();
    PyObjectRef pythonAnalyzerFolderName = PyUnicode_FromString(folderPath.toUtf8().data());
    if (PyList_Append(pythonSysPath.get(), pythonAnalyzerFolderName.get()) != 0) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось добавить путь к файлу Python модуля анализатора в"
                            "переменную \"sys.path\"."));
        return false;
    }

    return true;
}

bool Analyzer::_createPythonAnalyzerObject()
{
    QString fileName = _config->classConfig().pythonFileName();
    PyObjectRef analyzerPythonModuleName = PyUnicode_FromString(fileName.toUtf8().data());
    _analyzerPythonModule = PyImport_Import(analyzerPythonModuleName.get());
    if (!_analyzerPythonModule.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось загрузить файл Python модуля анализатора."));
        return false;
    }

    // Function PyModule_GetDict returns borrowed reference,
    // so we don't need to decreement reference count explicitly
    PyObject *analyzerPythonModuleDictionary = PyModule_GetDict(_analyzerPythonModule.get());
    if (!analyzerPythonModuleDictionary) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось загрузить словарь Python модуля анализатора."));
        return false;
    }

    // Function PyDict_GetItemString returns borrowed reference,
    // so we don't need to decreement reference count explicitly
    QString className = _config->classConfig().pythonClassName();
    PyObject *analyzerPythonClass = PyDict_GetItemString(
        analyzerPythonModuleDictionary,
        className.toUtf8().data()
    );
    if (!analyzerPythonClass) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось найти класс анализатора в Python модуле."));
        return false;
    }

    PyObjectRef args = PyTuple_New(0);
    PyObjectRef keywordArgs = _createInstanceInitDictionary();

    if (!args.get() || !keywordArgs.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось инициализировать параметры "
                            "экземпляра класса анализатора из Python модуля."));
        return false;
    }

    _analyzerPythonClassInstance = PyObject_Call(analyzerPythonClass, args.get(), keywordArgs.get());

    if (!_analyzerPythonClassInstance.get()) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось создать "
                            "экземпляр класса анализатора из Python модуля."));
        return false;
    }

    return true;
}

PyObject *Analyzer::_createInstanceInitDictionary()
{
    PyObject *keywordArgs = PyDict_New();

    qDebug() << "Analyzer init params:";
    const QJsonObject params = _config->allInitParams();
    for (const QString &paramName : params.keys()) {
        PyObject *paramValue = _jsonValueToPythonObject(params[paramName]);
        if (paramValue != nullptr) {
            PyDict_SetItemString(keywordArgs, paramName.toUtf8().data(), paramValue);
            qDebug() << paramName << params[paramName];
        }
    }
    qDebug() << "";

    return keywordArgs;
}

PyObject *Analyzer::_jsonValueToPythonObject(const QJsonValue &jsonValue)
{
    PyObject *value = nullptr;

    switch (jsonValue.type()) {
        case QJsonValue::Null:
            value = Py_None;
            break;
        case QJsonValue::Bool:
            value = (jsonValue.toBool() ? Py_True : Py_False);
            break;
        case QJsonValue::Double: {
            QVariant variantValue = jsonValue.toVariant();

            // From Qt Docs:
            //  Although function "QVariant::type" is declared as returning "QVariant::Type",
            //  the return value should be interpreted as "QMetaType::Type"."
            QMetaType::Type type = QMetaType::Type(variantValue.type());
            if (type == QMetaType::Float || type == QMetaType::Double) {
                value = PyFloat_FromDouble(variantValue.toDouble());
            } else {
                value = PyLong_FromLongLong(variantValue.toLongLong());
            }
            break;
        }
        case QJsonValue::String:
            value = PyUnicode_FromString(jsonValue.toString().toUtf8().data());
            break;
        case QJsonValue::Undefined:
            break;
        // TODO: [tvz] implement arrays/objects processing?
        default:
            Q_ASSERT_X(false, "Analyzer::_jsonValueToPythonObject", "unsupported QJsonValue type");
            break;
    }

    return value;
}

bool Analyzer::_initializeAnalyzerCallbacks()
{
    _onFitProgressMethodDef = std::make_unique<PyMethodDef>();
    _onFitProgressMethodDef->ml_doc = nullptr;
    _onFitProgressMethodDef->ml_flags = METH_VARARGS;
    _onFitProgressMethodDef->ml_name = _onFitProgressMethodName;
    _onFitProgressMethodDef->ml_meth = reinterpret_cast<PyCFunction>(&Analyzer::_onFitProgress);

    PyObject *builtInModule = PyImport_AddModule("builtins");

    PyObject *pythonFunc = PyCFunction_New(_onFitProgressMethodDef.get(), nullptr);
    if (!pythonFunc) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось создать Python объект \"on_fit_progress\"."));
        return false;
    }

    if (PyObject_SetAttrString(builtInModule, _onFitProgressMethodName, pythonFunc) != 0) {
        _writePythonErrorMessages();
        onError(QObject::tr("Ошибка: не удалось добавить Python объект \"on_fit_progress\" "
                            "в модуль \"builtins\"."));
        return false;
    }

    if (PyRun_SimpleString("import builtins") != 0) {
        onError(QObject::tr("Ошибка: не удалось импортировать Python модуль \"builtins\"."));
    }

    return true;
}

void Analyzer::_writePythonErrorMessages()
{
    PyErr_Print();

#if !defined(ENABLE_REDIRECTION_PYTHON_STDERR_TO_STRING)
    return;
#else

    PyObject *pythonSysModule = _pythonModules["sys"].get();

    Q_ASSERT(pythonSysModule != nullptr);

    PyObject *pythonStdErr = PyObject_GetAttrString(pythonSysModule, "stderr");
    if (!pythonStdErr) {
        onError(QObject::tr("Ошибка: не удалось получить Python объект \"sys.stderr\"."));
        return;
    }

    PyObject *pythonStdErrData = PyObject_CallMethod(pythonStdErr, "getvalue", nullptr);
    if (!pythonStdErrData) {
        onError(QObject::tr("Ошибка: не удалось получить содержимое буфера \"sys.stderr\"."));
        return;
    }

    PyObject *pythonStdErrEncodedData = PyUnicode_AsEncodedString(pythonStdErrData, "utf-8", "strict");
    if (!pythonStdErrEncodedData) {
        // TODO: [ProvkinVA] - handle exception when encoding string.
    }

    char *stdErrStringData = nullptr;
    Py_ssize_t stdErrStringDataSize = 0;
    if (PyBytes_AsStringAndSize(pythonStdErrEncodedData, &stdErrStringData, &stdErrStringDataSize) != 0) {
        onError(QObject::tr("Ошибка: не удалось получить указатель на данные \"sys.stderr\"."));
        return;
    }

    if (!stdErrStringData) {
        onError(QObject::tr("Ошибка: указатель на данные \"sys.stderr\" не должен быть пустым."));
        return;
    }

    // Currently all error messages from python put in string variable.
    // Need to pass them to GUI.
    _errorMessagesBuffer.append(QString(stdErrStringData));

    // TODO: [ProvkinVA] - remove this (it's only for debugging)
    qDebug().nospace() << "stdErrStringData = " << stdErrStringData;
#endif
}

void Analyzer::_addMessageToBuffer(const QString &message)
{
    _errorMessagesBuffer.append(message + "\r\n");
}

PyObject *Analyzer::_onFitProgress(PyObject */*self*/, PyObject *arg)
{
    _wasFitProgressCallbackCalled = true;

    if (PyTuple_Size(arg) == 0) {
        emit Observer.error(QObject::tr("Ошибка: функция \"on_fit_progress\" "
                               "была вызвана без аргументов. Требуется хотя бы один аргумент."));
        return Py_True;
    }

    PyObject *firstArg = PyTuple_GetItem(arg, 0);

    if (!PyFloat_Check(firstArg) && !PyLong_Check(firstArg)) {
        emit Observer.error(QObject::tr("Ошибка: первый аргумент функции \"on_fit_progress\" "
                               "не является числом."));
        return Py_True;
    }

    double fitProgress = PyFloat_AsDouble(firstArg);

    emit Observer.fitProgressChanged(ceilf(fitProgress * 100.0));
    QCoreApplication::processEvents(); // FIXME: [tvz] remove after threading implement

    return Py_True;
}
#undef __CLASS__
