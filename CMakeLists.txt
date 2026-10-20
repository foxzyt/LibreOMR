#
# LibreOMR - Robust Optical Mark Recognition Library
# Copyright (C) 2026 Bernardo Alvim
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.16)
project(LibreOMR
    VERSION 1.1.2
    DESCRIPTION "Robust Optical Mark Recognition (OMR) Library in C++ and C"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set standard RPATH so installed binaries automatically find their libraries
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(LIBREOMR_BUILD_CLI "Build LibreOMR CLI executable" ON)
option(LIBREOMR_BUILD_TESTS "Build LibreOMR unit tests" ON)

# Library sources
set(LIBREOMR_HEADERS
    include/libreomr/export.h
    include/libreomr/version.h
    include/libreomr/types.h
    include/libreomr/geometry.hpp
    include/libreomr/image.hpp
    include/libreomr/binarization.hpp
    include/libreomr/connected_components.hpp
    include/libreomr/fiducial.hpp
    include/libreomr/template.hpp
    include/libreomr/result.hpp
    include/libreomr/processor.hpp
    include/libreomr/json.hpp
    include/libreomr/libreomr.hpp
    include/libreomr/libreomr.h
)

set(LIBREOMR_SOURCES
    src/version.cpp
    src/geometry.cpp
    src/image.cpp
    src/image_io.cpp
    src/binarization.cpp
    src/connected_components.cpp
    src/fiducial.cpp
    src/template.cpp
    src/result.cpp
    src/processor.cpp
    src/json.cpp
    src/c_api.cpp
)

add_library(libreomr ${LIBREOMR_SOURCES} ${LIBREOMR_HEADERS})
add_library(LibreOMR::libreomr ALIAS libreomr)

target_include_directories(libreomr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(libreomr SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_compile_definitions(libreomr
    PRIVATE
        LIBREOMR_EXPORTS
)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(libreomr PUBLIC LIBREOMR_STATIC_DEFINE)
endif()

if(MSVC)
    target_compile_options(libreomr PRIVATE /W4 /permissive-)
else()
    target_compile_options(libreomr PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(libreomr PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "libreomr"
)

# CLI Tool
if(LIBREOMR_BUILD_CLI)
    add_executable(libreomr-cli cli/main.cpp)
    target_link_libraries(libreomr-cli PRIVATE libreomr)
    target_include_directories(libreomr-cli PRIVATE include)
    set_target_properties(libreomr-cli PROPERTIES OUTPUT_NAME "libreomr")
endif()

# Unit Tests
if(LIBREOMR_BUILD_TESTS)
    enable_testing()
    add_executable(libreomr_tests
        tests/main.cpp
        tests/test_geometry.cpp
        tests/test_image.cpp
        tests/test_fiducial.cpp
        tests/test_template.cpp
        tests/test_pipeline.cpp
        tests/test_c_api.cpp
    )
    target_link_libraries(libreomr_tests PRIVATE libreomr)
    target_include_directories(libreomr_tests PRIVATE include tests)
    add_test(NAME LibreOMRTests COMMAND libreomr_tests)
endif()

# Installation rules
install(TARGETS libreomr
    EXPORT LibreOMRTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/libreomr
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(LIBREOMR_BUILD_CLI)
    install(TARGETS libreomr-cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# CMake config exports
install(EXPORT LibreOMRTargets
    FILE LibreOMRTargets.cmake
    NAMESPACE LibreOMR::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibreOMR
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LibreOMRConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LibreOMRConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LibreOMRConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibreOMR
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LibreOMRConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LibreOMRConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibreOMR
)

# pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libreomr.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libreomr.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libreomr.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
