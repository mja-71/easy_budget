#
# CPack custom configuration for the selected generator
# (use option -D CPACK_PROJECT_CONFIG_FILE="cpack_generator.cmake"
#

if (CPACK_GENERATOR MATCHES "NSIS")
    set(CPACK_PACKAGE_FILE_NAME "EasyBudget_1.0-win64")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "EasyBudget")
    message("Generate package for WINDOWS")
elseif (CPACK_GENERATOR MATCHES "DEB")
    message("Generate package for DEBIAN")
    set(CPACK_PACKAGE_FILE_NAME "EasyBudget_1.0")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/easybudget")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "michel.janton@gmail.com")
else()
    message("Generate default package ${CPACK_GENERATOR}")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/easybudget")
endif()

