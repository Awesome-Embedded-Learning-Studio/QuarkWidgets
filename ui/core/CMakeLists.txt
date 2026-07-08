project(ui_core LANGUAGES CXX)

log_info("UI_Core" "Start Configure the UI Core")

# Include subdirectory CMakeLists (STATIC libraries)
add_subdirectory(material)

# ============================================================
# Unified UI Core Library - STATIC library for quarkwidgets.dll
# ============================================================

# STATIC library for linking into quarkwidgets.dll
# Note: color_scheme.h is a header-only interface file
add_library(qw_ui_core STATIC
    theme_manager.cpp
)

# Link against material sub-module and base
target_link_libraries(qw_ui_core PUBLIC
    qw_ui_core_material
    qw_ui_base
)

# Add include directories for header-only files
target_include_directories(qw_ui_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link Qt for compile dependencies
target_link_libraries(qw_ui_core PUBLIC
    Qt6::Core
    Qt6::Gui
    aex::aex
)

log_info("UI_Core" "the UI Core Configuration Done")
