##################################################################
#
# Unit Test build recipe
#
# This CMake file contains the recipe for building cFS app unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
# Alan S. Gibson, GSFC-587
#
# By convention this cmake file as written requires:
#
# 1. For each source file in an app there must be a
#    corresponding <src_filename>_test.c file in the
#    ${PROJECT_SOURCE_DIR}/unit-test directory
#    (the same directory that this cmake file should exist in)
#
# 2. For each source file in an app there must be a
#    corresponding <src_filename>_stub.c file in the
#    ${PROJECT_SOURCE_DIR}/unit-test/stubs directory
#
# Optionally you may have:
#
# 1. ${PROJECT_SOURCE_DIR}/unit-test/utilities directory that
#    contains any test specific utility files
#
# 2. ${PROJECT_SOURCE_DIR}/unit-test/inc directory that
#    contains any test specific header files
#
##################################################################

# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
# ${osal_MISSION_DIR}/ut_assert/inc is location of ut_assert headers and is required
include_directories(${osal_MISSION_DIR}/ut_assert/inc)
# ${CMAKE_CURRENT_SOURCE_DIR}/utilities is location of app's unit test utilities
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utilities)
# ${CMAKE_CURRENT_SOURCE_DIR}/stubs is location of app's source stub files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stubs)

# Get all the source filenames from stub subdirectory for app unit tests
set(app_stub_files)
set(app_stub_and_utility_filenames)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/stubs" app_stub_files)
foreach(SOURCE_FILE ${app_stub_files})
  list(APPEND app_stub_and_utility_filenames "${SOURCE_FILE}")
endforeach()

# Get the source filenames in the application's unit-test/utilities directory
# utility files are prepended by design, this ensures the correct build order for CF only - other milage may vary
set(app_ut_utility_files)
set(app_ut_utility_filenames)
aux_source_directory("${PROJECT_SOURCE_DIR}/unit-test/utilities" app_ut_utility_files)
foreach(SOURCE_FILE ${app_ut_utility_files})
  list(INSERT app_stub_and_utility_filenames 0 ${SOURCE_FILE})
endforeach()

# add both utility and stubs to the coverage stubs
# TODO: currently adding both is considered a work-around
# utilities should be added separately when a method for it is devised
add_cfe_coverage_stubs(cf ${app_stub_and_utility_filenames})

# Generate a dedicated "testrunner" executable for each test file
# Accomplish this by cycling through all the app's source files,
# there must be a *_tests file for each
set(source_files_under_test)
aux_source_directory("${PROJECT_SOURCE_DIR}/fsw/src" source_files_under_test)
foreach(SRCFILE ${source_files_under_test})

    # Get the base sourcefile name as a module name without path or the
    # extension, this will be used as the base name of the unit test file.
    get_filename_component(MODULE_NAME "${SRCFILE}" NAME_WE)

    # Use the module name to make the tests' name by adding _tests to the end
    set(TESTS_NAME "${MODULE_NAME}_tests")

    # Get the tests' sourcefile name with unit test path and extension
    set(TESTS_SOURCE_FILE "${PROJECT_SOURCE_DIR}/unit-test/${TESTS_NAME}.c")

    # build test, but CF includes the .c in the test's source therefore
    # the TESTCASE_SRC is left empty, while the UT_SRC is passed in as the
    # combined test and source (currently done to access 'static' functions)
    # without doing it like this the coverage reports will not be created
    add_cfe_coverage_test(cf "${TESTS_NAME}" "/" "${TESTS_SOURCE_FILE}")

    # give the dependencies of cf_app to the test, this ensures
    # app_stub_and_utility_filenames are used when building (TODO: ? is this comment correct?)
    add_cfe_coverage_dependency(cf "${TESTS_NAME}" cf)

endforeach()