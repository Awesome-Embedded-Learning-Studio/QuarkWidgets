project(ui_components LANGUAGES CXX)

log_info("UI_Components" "Start Configure the UI Components")

# ============================================================
# Base Animation Library - STATIC library for quarkwidgets.dll
# Contains base animation interfaces and implementations
# ============================================================

add_library(qw_ui_components STATIC
    animation.cpp
    animation_factory_manager.cpp
    timing_animation.cpp
)

# Add include directories
target_include_directories(qw_ui_components PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link Qt libraries
target_link_libraries(qw_ui_components PUBLIC
    Qt6::Core
    Qt6::Gui
    aex::aex
    qw_ui_base
)

# Include subdirectory CMakeLists (STATIC libraries)
# Material depends on qw_ui_components, so it must come after
add_subdirectory(material)

log_info("UI_Components" "the UI Components Configuration Done")
