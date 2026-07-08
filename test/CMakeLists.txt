# QuarkWidgets self-tests (QTest). Built only for standalone builds
# (QUARKWIDGETS_BUILD_TESTS, defaulting to PROJECT_IS_TOP_LEVEL); host projects
# pulling us in via add_subdirectory get no QuarkWidgets tests in their ctest.
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(test_quarkbulb test_quarkbulb.cpp)
target_link_libraries(test_quarkbulb PRIVATE QuarkWidgets Qt6::Test)
add_test(NAME test_quarkbulb COMMAND test_quarkbulb)

add_executable(test_ledpanel test_ledpanel.cpp)
target_link_libraries(test_ledpanel PRIVATE QuarkWidgets Qt6::Test)
add_test(NAME test_ledpanel COMMAND test_ledpanel)

# Headless: render the widgets offscreen (no X server needed).
set_tests_properties(test_quarkbulb test_ledpanel PROPERTIES
    ENVIRONMENT QT_QPA_PLATFORM=offscreen)
