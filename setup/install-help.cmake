#
# Help files install
# 

# find_package(Qt5 COMPONENTS Widgets Gui Core Network REQUIRED)
find_package(Qt6 COMPONENTS Widgets OpenGL REQUIRED)

# for assistant
find_package(Qt6 COMPONENTS Help Sql Network PrintSupport REQUIRED)

install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/help/easybudget.qhc"
               "${CMAKE_CURRENT_SOURCE_DIR}/help/easybudget.qch"
         DESTINATION bin/help
         COMPONENT "ACCOUNT_HELP"
)

# Install Qt assistant into "bin"
if (WIN32)
    install (FILES $<TARGET_FILE:Qt6::Help>
                   $<TARGET_FILE:Qt6::Sql>
                   $<TARGET_FILE:Qt6::PrintSupport>
             DESTINATION bin
             COMPONENT "ACCOUNT_HELP"
    )
    install (FILES "${Qt6_DIR}/../../../plugins/sqldrivers/qsqlite.dll"
        DESTINATION bin/sqldrivers
        COMPONENT "ACCOUNT_HELP"
    )
    # assistant
    install(FILES "${Qt6_DIR}/../../../bin/assistant.exe"
        DESTINATION bin
        COMPONENT "ACCOUNT_HELP"
    )

else()
    install (FILES
               $<TARGET_FILE:Qt6::Help>
               $<TARGET_FILE:Qt6::Sql>
         CONFIGURATIONS Release
         DESTINATION lib
         COMPONENT "LIB_QT"
    )
    # workaround add link to so file
    install (FILES ${Qt6_DIR}/../../libQt6Help.so.6
                   ${Qt6_DIR}/../../libQt6Sql.so.6
                   ${Qt6_DIR}/../../libQt6PrintSupport.so.6
             DESTINATION lib
             COMPONENT "ACCOUNT_HELP")
    # qt sql drivers
    install (DIRECTORY "${Qt6_DIR}/../../../plugins/sqldrivers"
        DESTINATION bin
        FILES_MATCHING PATTERN "*.so*"
    )
    # assistant
    install(FILES "${Qt6_DIR}/../../../bin/assistant"
        DESTINATION bin
        COMPONENT "ACCOUNT_HELP"
    )
endif()

