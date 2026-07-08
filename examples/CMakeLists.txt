# One minimal standalone app per concern — proves the library links & runs.
# Built only for standalone QuarkWidgets builds (QUARKWIDGETS_BUILD_EXAMPLES,
# defaulting to PROJECT_IS_TOP_LEVEL; see root CMakeLists.txt).
add_executable(quarkwidgets_example
    quarkwidgets_example.cpp
)
target_link_libraries(quarkwidgets_example PRIVATE QuarkWidgets)

# One bulb auto-toggling — minimal QuarkBulb demo.
add_executable(bulb_example
    bulb_example.cpp
)
target_link_libraries(bulb_example PRIVATE QuarkWidgets)

# An 8-LED panel cycling its ODR bitmask.
add_executable(ledpanel_example
    ledpanel_example.cpp
)
target_link_libraries(ledpanel_example PRIVATE QuarkWidgets)

# A serial terminal fed by a simulated UART stream (RX lines + TX acks).
add_executable(uartterminal_example
    uartterminal_example.cpp
)
target_link_libraries(uartterminal_example PRIVATE QuarkWidgets)
