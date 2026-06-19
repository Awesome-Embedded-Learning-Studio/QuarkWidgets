project(ui_components_material LANGUAGES CXX)

log_info("UI_Components_Material" "Start Configure the UI Components Material")

# STATIC library for linking into quarkwidgets.dll
add_library(qw_ui_components_material STATIC
    cfmaterial_fade_animation.cpp
    cfmaterial_slide_animation.cpp
    cfmaterial_scale_animation.cpp
    cfmaterial_property_animation.cpp
    cfmaterial_animation_factory.cpp
    cfmaterial_animation_strategy.cpp
)

# Add include directories
target_include_directories(qw_ui_components_material PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

# Link Qt for compile dependencies and base animation library
target_link_libraries(qw_ui_components_material PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    aex::aex
    qw_ui_base
    qw_ui_components
)

log_info("UI_Components_Material" "the UI Components Material Configuration Done")
