cmake_minimum_required(VERSION 3.15)

project(typst-library-manager VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(typst-libm src/typst-library-manager.cpp)

target_include_directories(typst-libm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/typst-library-manager
)

include(GNUInstallDirs)

install(TARGETS typst-libm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES LICENSE THIRD_PARTY_NOTICES.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

set(CPACK_PACKAGE_NAME "typst-library-manager")
set(CPACK_PACKAGE_VENDOR "Owe-plisplus")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Typst library manager CLI tool")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;TGZ") 
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "omoryuryu <omoryuryu1@gmail.com>")
endif()

include(CPack)
