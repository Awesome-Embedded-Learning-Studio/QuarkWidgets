cmake_minimum_required(VERSION 3.16)

# QuarkWidgets — Awesome Embedded Studio's reusable Qt6 Material 3 widget
# library. Consumed by CFDesktop (and other org projects) via add_subdirectory
# or as an installed package.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(QuarkWidgets VERSION 0.1.0 LANGUAGES CXX)
    set(QW_MASTER_PROJECT ON)
else()
    set(QW_MASTER_PROJECT OFF)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# quarkwidgets is a SHARED lib assembled from static sub-archives; they must be
# position-independent so the linker can fold them into libquarkwidgets.so.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 6.8.3 COMPONENTS Core Gui Widgets QUIET)

# ============================================================
# aex dependency (header-only foundation: weak_ptr, expected, ...)
# When consumed by a superproject that already provides aex::aex (e.g.
# CFDesktop), this is a no-op. Standalone builds fetch aex @ v0.1.0.
# ============================================================
if(NOT TARGET aex::aex)
    include(FetchContent)
    FetchContent_Declare(aex
        GIT_REPOSITORY https://github.com/Awesome-Embedded-Learning-Studio/aex.git
        GIT_TAG        v0.1.0)
    FetchContent_MakeAvailable(aex)
endif()

option(QW_BUILD_TESTS    "Build QuarkWidgets tests"    ${QW_MASTER_PROJECT})
option(QW_BUILD_EXAMPLES "Build QuarkWidgets examples" ${QW_MASTER_PROJECT})

# log_info no-op: the migrated CMakeLists use CFDesktop's log_info() macro.
if(NOT COMMAND log_info)
    macro(log_info _module _msg)
        message(STATUS "[${_module}] ${_msg}")
    endmacro()
endif()

add_subdirectory(ui)

if(QW_BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qw_add_test.cmake)
    add_subdirectory(test)
endif()

if(QW_BUILD_EXAMPLES)
    # CFDesktop-specific helpers used by migrated example CMakeLists; no-op here
    # (default output dir is fine for standalone QuarkWidgets examples).
    if(NOT COMMAND cf_set_example_output_dir)
        function(cf_set_example_output_dir TARGET_NAME CATEGORY)
        endfunction()
    endif()
    if(NOT COMMAND cf_register_example_launcher)
        function(cf_register_example_launcher)
        endfunction()
    endif()
    add_subdirectory(example)
endif()
