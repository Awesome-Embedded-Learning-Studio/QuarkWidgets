# QuarkWidgets — org-level Qt6 Widgets control library.
#
# Pure Qt, data-driven, QPainter self-drawn widgets. ZERO host-app dependencies:
# nothing in here may reference consumer types (no snapshot/EventBus/GpioEdge/…).
# Consumers (micro-forge, TAMCPP, …) feed plain Qt data in and render it here.
#
# Two consumption modes:
#   - Standalone:  cmake -B build -S .   (builds lib + example apps)
#   - Embedded:    add_subdirectory(<this>) from a host project (examples off)
cmake_minimum_required(VERSION 3.25)
project(QuarkWidgets LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)

# ---- Library ---------------------------------------------------------------
# STATIC: each consumer that add_subdirectory()'s us gets its own copy compiled
# into its build tree (micro-forge-gui, future TAMCPP host tools, …). B0 ships a
# single anchor TU proving the Qt6 + AUTOMOC + include-path wiring end to end;
# real widgets (QuarkBulb, LedPanel, UartTerminalView) are appended to the list.
add_library(QuarkWidgets STATIC
    src/quarkwidgets.cpp
)
add_library(QuarkWidgets::QuarkWidgets ALIAS QuarkWidgets)

target_include_directories(QuarkWidgets PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(QuarkWidgets PUBLIC cxx_std_23)
target_link_libraries(QuarkWidgets PUBLIC Qt6::Widgets)

# The library's own sources must compile clean (-Werror); PRIVATE so consumer
# translation units are not held to these flags.
if(NOT MSVC)
    target_compile_options(QuarkWidgets PRIVATE -Wall -Wextra -Werror)
endif()

# ---- Standalone examples (off when consumed via add_subdirectory) ----------
# PROJECT_IS_TOP_LEVEL is true only for a standalone QuarkWidgets build, so host
# projects pulling us in via add_subdirectory get no example binaries unless they
# opt in explicitly.
option(QUARKWIDGETS_BUILD_EXAMPLES "Build QuarkWidgets standalone example apps"
       ${PROJECT_IS_TOP_LEVEL})
if(QUARKWIDGETS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
