# QuarkWidgets — embedded-debug controls (QuarkBulb / LedPanel / UartTerminalView).
#
# A self-contained sub-library of the QuarkWidgets repo, deliberately isolated
# from the Material-3 widget library at the repo root. Different build
# architecture from the root: this one is STATIC + QTest + zero external deps;
# the root is SHARED + aex + GoogleTest + Qt 6.8.3. They never share a target.
#
# Consume it from a host project (e.g. micro-forge):
#   add_subdirectory("<repo>/embedded" "${CMAKE_BINARY_DIR}/quarkwidgets-embedded")
#   target_link_libraries(your-app PRIVATE QuarkWidgets::Embedded)
#
# Or build standalone:  cmake -B build -S embedded
#
# Namespace `quark`; include dir stays QuarkWidgets/ (consumers write
# #include "QuarkWidgets/QuarkBulb.hpp"). The target is named QuarkWidgetsEmbedded
# (alias QuarkWidgets::Embedded) so it cannot clash with the root library's
# `quarkwidgets` target should both ever land in one build tree.

# Always call cmake_minimum_required + project(): when embedded/ is built
# standalone this makes it a top-level project (so PROJECT_IS_TOP_LEVEL is true
# and the example/test options turn on); when consumed via add_subdirectory the
# nested project() is allowed by CMake and harmlessly tags a sub-project.
cmake_minimum_required(VERSION 3.25)
project(QuarkWidgetsEmbedded LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)

# ---- Library ---------------------------------------------------------------
add_library(QuarkWidgetsEmbedded STATIC
    src/quarkwidgets.cpp
    src/QuarkBulb.cpp
    src/LedPanel.cpp
    src/UartTerminalView.cpp
    include/QuarkWidgets/QuarkBulb.hpp        # Q_OBJECT → list header for AUTOMOC
    include/QuarkWidgets/LedPanel.hpp         # (same)
    include/QuarkWidgets/UartTerminalView.hpp # (same)
)
add_library(QuarkWidgets::Embedded ALIAS QuarkWidgetsEmbedded)

target_include_directories(QuarkWidgetsEmbedded PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(QuarkWidgetsEmbedded PUBLIC cxx_std_23)
target_link_libraries(QuarkWidgetsEmbedded PUBLIC Qt6::Widgets)

# The library's own sources must compile clean (-Werror); PRIVATE so consumer
# translation units are not held to these flags.
if(NOT MSVC)
    target_compile_options(QuarkWidgetsEmbedded PRIVATE -Wall -Wextra -Werror)
endif()

# ---- Standalone examples / tests (off when consumed via add_subdirectory) --
# PROJECT_IS_TOP_LEVEL is true only for a standalone embedded/ build, so host
# projects pulling us in get no example/test binaries unless they opt in.
option(QUARKWIDGETS_BUILD_EXAMPLES "Build the embedded-control example apps"
       ${PROJECT_IS_TOP_LEVEL})
if(QUARKWIDGETS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(QUARKWIDGETS_BUILD_TESTS "Build the embedded-control QTest self-tests"
       ${PROJECT_IS_TOP_LEVEL})
if(QUARKWIDGETS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
