CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(SignalAnalyzer VERSION 0.0.1 LANGUAGES CXX)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Widgets)
SET(QT_LIBS Qt5::Core Qt5::Widgets)
FIND_PACKAGE(Qwt REQUIRED)
FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter Development)

SET(SOURCES
    config/AbstractConfig.h                         config/AbstractConfig.cpp
    config/ApplicationConfig.h                      config/ApplicationConfig.cpp
    config/AnalyzerClassConfig.h                    config/AnalyzerClassConfig.cpp
    config/AnalyzerInstanceConfig.h                 config/AnalyzerInstanceConfig.cpp
    config/InitParam.h                              config/InitParam.cpp
    config/JsonKeys.h

    core/model/analyzer-facade.h                    core/model/analyzer-facade.cpp
    core/model/AnalyzerObserver.h
    core/model/analyzer.h                           core/model/analyzer.cpp
    core/model/ClassLabel.h                         core/model/ClassLabel.cpp
    core/model/event.h                              core/model/event.cpp
    core/model/events-model.h                       core/model/events-model.cpp
    core/model/general-analyzer.h                   core/model/general-analyzer.cpp
    core/model/py-object-ref.h                      core/model/py-object-ref.cpp
    core/model/scoped-python-interpreter.h          core/model/scoped-python-interpreter.cpp
    core/model/signal-buffer.h                      core/model/signal-buffer.cpp
    core/model/signal-chunk.h                       core/model/signal-chunk.cpp
    core/model/signal-model.h                       core/model/signal-model.cpp
    core/model/signal-fragment.h                    core/model/signal-fragment.cpp
    core/model/SignalPart.h                         core/model/SignalPart.cpp
    core/model/string-queue-model.h                 core/model/string-queue-model.cpp
    core/model/signal-types.h
    core/model/TimeInterval.h

    core/view/SignalPlot.h                          core/view/SignalPlot.cpp

    player/model/player-core.h                      player/model/player-core.cpp
    player/model/new/Player.h                       player/model/new/Player.cpp
    player/model/new/csvPlayer.h                    player/model/new/csvPlayer.cpp
    player/model/new/opcPlayer.h                    player/model/new/opcPlayer.cpp	
    player/model/csv-signal-fragment-reader.h       player/model/csv-signal-fragment-reader.cpp

    network/HistoryServerStub.h                     network/HistoryServerStub.cpp
    network/new/HistoryData.h                       network/new/HistoryData.cpp
    network/new/HistoryDataCsvFile.h                network/new/HistoryDataCsvFile.cpp
    network/new/HistoryDataOpcClient.h              network/new/HistoryDataOpcClient.cpp

    core/controller/AnalyzerInstancePropertiesDialog.h
    core/controller/AnalyzerInstancePropertiesDialog.cpp

    core/controller/analyzer-widget.h               core/controller/analyzer-widget.cpp
    core/controller/CreateAnalyzerInstanceDialog.h  core/controller/CreateAnalyzerInstanceDialog.cpp
    core/controller/main-widget.h                   core/controller/main-widget.cpp
    core/controller/TimeIntervalSetupWidget.h       core/controller/TimeIntervalSetupWidget.cpp

    core/controller/analyzer-widget.ui
    core/controller/CreateAnalyzerInstanceDialog.ui
    core/controller/main-widget.ui

    player/controller/PlayerControllerWidget.h    player/controller/PlayerControllerWidget.cpp
    player/controller/PlayerControllerWidget.ui

    main.cpp
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES}
    core/controller/HistoryDataSourceChooseDialog.h core/controller/HistoryDataSourceChooseDialog.cpp core/controller/HistoryDataSourceChooseDialog.ui
    player/controller/PlayerChooseDialog.h player/controller/PlayerChooseDialog.cpp player/controller/PlayerChooseDialog.ui

)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBS} ${QWT_LIBRARIES} ${Python3_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${QWT_INCLUDE_DIR} ${Python3_INCLUDE_DIRS})



SET(ANALYZERS_DIR ${CMAKE_SOURCE_DIR}/analyzers)
SET(BUILD_ANALYZERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/analyzers)

FILE(MAKE_DIRECTORY ${BUILD_ANALYZERS_DIR})

IF(NOT EXISTS ${BUILD_ANALYZERS_DIR}/classes)
    FILE(CREATE_LINK ${ANALYZERS_DIR}/classes ${BUILD_ANALYZERS_DIR}/classes SYMBOLIC)
ENDIF()

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/settings.json)
    FILE(COPY_FILE settings.json ${CMAKE_CURRENT_BINARY_DIR}/settings.json)
ENDIF()
