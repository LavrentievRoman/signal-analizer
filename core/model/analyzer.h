#pragma once

#include <memory>

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "config/AnalyzerInstanceConfig.h"

#include "AnalyzerObserver.h"
#include "py-object-ref.h"

extern "C" typedef struct PyMethodDef PyMethodDef;

class AnalyzerInstanceConfig;

class Analyzer
{
public:
    static AnalyzerObserver Observer; //TODO: [tvz] replace with a pointer to GeneralAnalyzer?

    Analyzer(std::shared_ptr<AnalyzerInstanceConfig> config);
    ~Analyzer();

    AnalyzerState state() const;

    bool isLoaded() const;
    bool load();
    void unload();

    void fit(const QVector<double> &trainData);
    double estimate(const QVector<double> &inputData);

    const QString &name() const { return _config->name(); }
    quint32 inputSize() const { return _analyzerInputSize; }

private:
    PyObject *_createInstanceInitDictionary();
    PyObject *_jsonValueToPythonObject(const QJsonValue &jsonValue);
    void onError(QString message);
    void setState(AnalyzerState state);
    std::shared_ptr<AnalyzerInstanceConfig> _config;

    QMap<QString, PyObjectRef> _pythonModules;

    PyObjectRef _analyzerPythonModule;
    PyObjectRef _analyzerPythonClassInstance;

    quint32 _analyzerInputSize;

    bool _isAnalyzerLoaded;

    /**
     * Current analyzer state. Must chagnes only via the "setState" method.
     */
    AnalyzerState _state;

    static const constexpr char *_fitMethodName = "fit";
    static const constexpr char *_estimateMethodName = "estimate";

    QString _errorMessagesBuffer;

    QVector<QString> _requiredPythonModules{ "sys", "io" };

    PyObject *_loadPythonModule(const QString &moduleName);
    bool _loadRequiredPythonModules();

    bool _redirectPythonStdErrToString();

    bool _addModuleFolderToSysPath();
    bool _createPythonAnalyzerObject();
    bool _initializeAnalyzerCallbacks();

    void _writePythonErrorMessages();
    void _addMessageToBuffer(const QString &message);

    std::unique_ptr<PyMethodDef> _onFitProgressMethodDef;
    static PyObject *_onFitProgress(PyObject *self, PyObject *arg);

    static const constexpr char *_onFitProgressMethodName = "on_fit_progress";

    static bool _wasFitProgressCallbackCalled;
};
