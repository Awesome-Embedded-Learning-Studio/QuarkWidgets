# add_gtest_executable — minimal GoogleTest target helper for QuarkWidgets.
# Compatible subset of CFDesktop's helper (same argument names) so the migrated
# test CMakeLists work unchanged.
#
#   add_gtest_executable(
#       TEST_NAME      <exe>
#       SOURCE_FILE    <src.cpp>
#       LINK_LIBRARIES <lib;...>
#       LABELS         "a;b;c"
#       LOG_MODULE     <name>   # accepted, ignored
#   )
function(add_gtest_executable)
    set(oneValueArgs TEST_NAME SOURCE_FILE LOG_MODULE)
    set(multiValueArgs LINK_LIBRARIES LABELS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${ARG_TEST_NAME} ${ARG_SOURCE_FILE})
    target_link_libraries(${ARG_TEST_NAME} PRIVATE ${ARG_LINK_LIBRARIES} GTest::gtest_main)
    target_compile_features(${ARG_TEST_NAME} PRIVATE cxx_std_23)
    if(ARG_LABELS)
        gtest_discover_tests(${ARG_TEST_NAME} PROPERTIES LABELS "${ARG_LABELS}")
    else()
        gtest_discover_tests(${ARG_TEST_NAME})
    endif()
endfunction()
