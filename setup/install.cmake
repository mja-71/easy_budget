#
# file install
# 

if (UNIX)
    set_target_properties(AccountTool
                          PROPERTIES INSTALL_RPATH "$ORIGIN/../lib"
    )

    set_target_properties(AccountToolData
                          PROPERTIES INSTALL_RPATH "$ORIGIN"
    )

    install (TARGETS AccountTool
             RUNTIME DESTINATION bin
             COMPONENT "ACCOUNT_TOOL"
    )
    install (TARGETS AccountToolData
              LIBRARY DESTINATION lib
              COMPONENT "ACCOUNT_TOOL"
    )

else ()
    install (TARGETS AccountTool
             RUNTIME DESTINATION bin
             COMPONENT "ACCOUNT_TOOL"
    )
    install (TARGETS AccountToolData
              LIBRARY DESTINATION bin
              COMPONENT "ACCOUNT_TOOL"
    )
endif()

if (NOT EXISTS "${CMAKE_INSTALL_PREFIX}/bin/config.xml")
    install (FILES "${CMAKE_CURRENT_LIST_DIR}/config/config.xml"
             DESTINATION bin
             COMPONENT "ACCOUNT_TOOL"
    )
endif()
