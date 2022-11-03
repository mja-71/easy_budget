#
# qt file install
# 

# find_package(Qt5 COMPONENTS Widgets Gui Core Network REQUIRED)
find_package(Qt6 COMPONENTS Widgets OpenGL REQUIRED)

# for assistant
find_package(Qt6 COMPONENTS Help Sql Network PrintSupport REQUIRED)

# Install Qt library into "bin"
if (WIN32)
    # basic components
    install (FILES $<TARGET_FILE:Qt6::Core>
                   $<TARGET_FILE:Qt6::Gui>
                   $<TARGET_FILE:Qt6::Widgets>
             DESTINATION bin
             COMPONENT "LIB_QT"
    )
    install (FILES "${Qt6_DIR}/../../../plugins/platforms/qwindows.dll"
        CONFIGURATIONS Release
        DESTINATION bin/platforms
        COMPONENT "LIB_QT"
    )
    install (FILES "${Qt6_DIR}/../../../plugins/platforms/qwindowsd.dll"
        CONFIGURATIONS Debug
        DESTINATION bin/platforms
        COMPONENT "LIB_QT"
    )
    # plugins: image formats
    install (FILES "${Qt6_DIR}/../../../plugins/imageformats/qsvgd.dll"
        CONFIGURATIONS Debug
        DESTINATION bin/imageformats
        COMPONENT "LIB_QT"
    )
else()
    install (FILES $<TARGET_FILE:Qt6::Core>
               $<TARGET_FILE:Qt6::Gui>
               $<TARGET_FILE:Qt6::Widgets>
               $<TARGET_FILE:Qt6::OpenGL>
               $<TARGET_FILE:Qt6::Help>
               $<TARGET_FILE:Qt6::Sql>
               $<TARGET_FILE:Qt6::Network>
               $<TARGET_FILE:Qt6::PrintSupport>
         CONFIGURATIONS Release
         DESTINATION lib
         COMPONENT "LIB_QT"
    )
    # plugins: image formats
    install (FILES "${Qt6_DIR}/../../../plugins/imageformats/libqsvg.so"
             CONFIGURATIONS Release
             DESTINATION bin/imageformats
             COMPONENT "LIB_QT"
    )
    # workaround add link to so file
    install (FILES ${Qt6_DIR}/../../libQt6Core.so.6
                   ${Qt6_DIR}/../../libQt6Gui.so.6
                   ${Qt6_DIR}/../../libQt6Widgets.so.6
                   ${Qt6_DIR}/../../libQt6OpenGL.so.6
                   ${Qt6_DIR}/../../libQt6Help.so.6
                   ${Qt6_DIR}/../../libQt6Network.so.6
                   ${Qt6_DIR}/../../libQt6Sql.so.6
                   ${Qt6_DIR}/../../libQt6PrintSupport.so.6
             DESTINATION lib
             COMPONENT "LIB_QT")
    # qt platforms
    install (DIRECTORY "${Qt6_DIR}/../../../plugins/platforms"
        DESTINATION bin
        FILES_MATCHING PATTERN "*.so*"
    )
    # qt sql drivers
    install (DIRECTORY "${Qt6_DIR}/../../../plugins/sqldrivers"
        DESTINATION bin
        FILES_MATCHING PATTERN "*.so*"
    )
    # extra library
    install (DIRECTORY "${Qt6_DIR}/../../"
        DESTINATION lib
        FILES_MATCHING PATTERN "libicui18n.so*"
    )
    install (DIRECTORY "${Qt6_DIR}/../../"
        DESTINATION lib
        FILES_MATCHING PATTERN "libicuuc.so*"
    )
    install (DIRECTORY "${Qt6_DIR}/../../"
        DESTINATION lib
        FILES_MATCHING PATTERN "libicudata.so*"
    )
    install (DIRECTORY "${Qt6_DIR}/../../"
        DESTINATION lib
        FILES_MATCHING PATTERN "libQt6XcbQpa.so.*"
    )
    # assistant
    install(FILES "${Qt6_DIR}/../../../bin/assistant"
        DESTINATION bin
        COMPONENT "LIB_QT"
    )
endif()

