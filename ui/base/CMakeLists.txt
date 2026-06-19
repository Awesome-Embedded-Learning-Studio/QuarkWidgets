project(ui_base LANGUAGES CXX)

log_info("UI_Base" "Start Configure the UI Base")

# STATIC library for linking into quarkwidgets.dll
add_library(qw_ui_base STATIC
    math_helper.cpp
    color_helper.cpp
    color.cpp
    device_pixel.cpp
    geometry_helper.cpp
    easing.cpp
)

# Add include directories so headers can be found with ui/base/ prefix
# We need to add the ui parent directory so that ui/base/math_helper.h works
target_include_directories(qw_ui_base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    $<INSTALL_INTERFACE:include>
)

# Link Qt for compile dependencies (headers, etc.)
target_link_libraries(qw_ui_base PUBLIC Qt6::Core Qt6::Gui)

log_info("UI_Base" "the UI Base Configuration Done")
