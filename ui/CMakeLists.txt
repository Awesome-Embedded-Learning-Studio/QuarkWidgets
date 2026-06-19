project(quarkwidgets_ui DESCRIPTION "QuarkWidgets UI Components" LANGUAGES CXX)

log_info("UI" "Start UI Components Configurations")

# All sources under ui/ are compiled into quarkwidgets.dll, so define quarkwidgets_EXPORTS
# so that QW_EXPORT expands to __declspec(dllexport) instead of dllimport.
add_compile_definitions(quarkwidgets_EXPORTS)

# Base Supports the UI Components Needs
add_subdirectory(base)

# Core Provides the UI Components Supports
add_subdirectory(core)

# Components Provides animation and UI component implementations
add_subdirectory(components)

# Widget Supports the Middle Level Widgets Abstractions
add_subdirectory(widget)

# Models will provide SDK Like Models for programmers
# Quick Start Apps
add_subdirectory(models)

# ============================================================
# Unified UI Library - Static Library
# ============================================================

add_library(quarkwidgets SHARED)

set(QW_STATIC_LIBS
    qw_ui_base
    qw_ui_core
    qw_ui_core_material
    qw_ui_widget_material
    qw_ui_application_support
    qw_ui_components_material
    qw_ui_components
)

# Force all objects into quarkwidgets.dll (PRIVATE: only affects quarkwidgets's own link step).
# quarkwidgets has no own sources, so the linker would otherwise skip static lib objects.
# Note: qw_ui_widget is an INTERFACE library — use its actual static lib deps instead.
if(MSVC)
    target_link_libraries(quarkwidgets PRIVATE ${QW_STATIC_LIBS})
    foreach(_qw_module IN LISTS QW_STATIC_LIBS)
        target_link_options(quarkwidgets PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE:${_qw_module}>")
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE)
    target_link_libraries(quarkwidgets PRIVATE
        -Wl,--whole-archive
        ${QW_STATIC_LIBS}
        -Wl,--no-whole-archive
    )
else()
    target_link_libraries(quarkwidgets PRIVATE ${QW_STATIC_LIBS})
endif()

# Set include directories
target_include_directories(quarkwidgets PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components/material>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widget>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/models>
    $<INSTALL_INTERFACE:include>
)

# Link Qt + aex foundation (weak_ptr etc.)
target_link_libraries(quarkwidgets PUBLIC
    Qt6::Core
    Qt6::Gui
    aex::aex
)

# Public alias consumed by downstream projects (e.g. CFDesktop)
add_library(QuarkWidgets::quarkwidgets ALIAS quarkwidgets)

log_info("UI" "Start UI Components Configurations Done")
