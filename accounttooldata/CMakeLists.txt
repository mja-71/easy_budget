
cmake_minimum_required(VERSION 3.5)

project(AccountToolData VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_RPATH "$ORIGIN")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml REQUIRED)

set(PROJECT_SOURCES
    accounttooldata.cpp
    operation.cpp
    accountmgr.cpp
    dataaccountfile.cpp
    budgetitem.cpp
    statement.cpp
    statefilece.cpp
    accountsynthesis.cpp
    statefileofx.cpp
    statepos.cpp
    accountmerger.cpp
    period.cpp
    AccountToolData_global.h
    operation.h
    dataaccount.h
    accountmgr.h
    dataaccountfile.h
    budgetitem.h
    statement.h
    statefilece.h
    accountsynthesis.h
    statefileofx.h
    statepos.h
    accountmerger.h
    period.h
    istatefile.h
    statefilefactory.h statefilefactory.cpp
)

add_library(AccountToolData SHARED ${PROJECT_SOURCES})
target_compile_definitions(AccountToolData PRIVATE ACCOUNTTOOLDATA_LIBRARY)
target_link_libraries(AccountToolData PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml )


