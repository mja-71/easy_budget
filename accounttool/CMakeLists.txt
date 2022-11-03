
cmake_minimum_required(VERSION 3.5)

project(AccountTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_RPATH "$ORIGIN")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp 
    panelreleve.cpp 
    panelsynthese.cpp 
    modeloplist.cpp     
    comboboxdelegate.cpp 
    dlgnewop.cpp 
    appconfig.cpp 
    panelitems.cpp 
    modelitems.cpp 
    dlgnewstate.cpp 
    editopdelegate.cpp 
    modelmergelist.cpp 
    delegatemergeitem.cpp 
    modeloplistproxy.cpp 
    delegatefilteritem.cpp 
    dlgupdateitemoptions.cpp 
    appsetting.cpp
    mainwindow.h 
    panelreleve.h 
    panelsynthese.h 
    modeloplist.h 
    comboboxdelegate.h 
    dlgnewop.h 
    appconfig.h 
    panelitems.h 
    modelitems.h 
    dlgnewstate.h 
    editopdelegate.h 
    modelmergelist.h 
    delegatemergeitem.h 
    modeloplistproxy.h 
    delegatefilteritem.h 
    dlgupdateitemoptions.h 
    appsetting.h
)

set(PROJECT_UI
    dlgnewop.ui
    dlgnewstate.ui
    dlgupdateitemoptions.ui
    mainwindow.ui
    panelitems.ui
    panelreleve.ui
    panelsynthese.ui
    resource.qrc
)


add_executable(AccountTool WIN32 ${PROJECT_SOURCES} ${PROJECT_UI})
target_link_libraries(AccountTool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets AccountToolData)


