#pragma once

namespace JsonKeys
{
namespace ApplicationConfig
{
    constexpr const char *jk_currentAnalyzerInstance    = "currentAnalyzerInstance";
    constexpr const char *jk_timeInterval               = "timeInterval";
    constexpr const char *jk_timeIntervalValue          = "value";
    constexpr const char *jk_timeIntervalUnits          = "units";
    constexpr const char *jk_operativeServer            = "operativeServer";
    constexpr const char *jk_historyServer              = "historyServer";
    constexpr const char *jk_valuesPerSec               = "valuesPerSec";
    constexpr const char *jk_serverSource               = "source";
}

namespace AnalyzerClassConfig
{
    constexpr const char *jk_name                       = "name";
    constexpr const char *jk_description                = "description";
    constexpr const char *jk_folderName                 = "folderName";
    constexpr const char *jk_pythonFileName             = "pythonFileName";
    constexpr const char *jk_pythonClassName            = "pythonClassName";
    constexpr const char *jk_initParams                 = "initParams";
}

namespace AnalyzerInstanceConfig
{
    constexpr const char *jk_name                       = "name";
    constexpr const char *jk_fitSource                  = "fitSource";
    constexpr const char *jk_stateSaved                 = "stateSaved";
    constexpr const char *jk_step                       = "step";
    constexpr const char *jk_threshold                  = "threshold";
    constexpr const char *jk_initParams                 = "initParams";
}

namespace InitParam
{
    constexpr const char *jk_name                       = "name";
    constexpr const char *jk_variableName               = "variableName";
    constexpr const char *jk_defaultValue               = "defaultValue";
    constexpr const char *jk_description                = "description";
}
}
