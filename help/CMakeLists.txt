
cmake_minimum_required(VERSION 3.5)

project(AccountToolHelp VERSION 0.1 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Help REQUIRED)

set(SRC_HELP_FILES   easybudget.qhcp
                    easybudget.qhp
                    help-gen.html
                    help-vue-releves.html
                    help-vue-categories.html
                    help-vue-synthese.html
                    help-import-releve.html
)

add_custom_target(AccountToolHelp ALL
    SOURCES
      ${SRC_HELP_FILES}
      easybudget.qch
)

add_custom_command(OUTPUT
                      ${CMAKE_CURRENT_SOURCE_DIR}/easybudget.qch
                   DEPENDS
                      ${SRC_HELP_FILES}
                   COMMAND
                      ${Qt6_DIR}/../../../bin/qhelpgenerator ${CMAKE_CURRENT_SOURCE_DIR}/easybudget.qhcp
                   COMMENT "Generate HELP"
)

