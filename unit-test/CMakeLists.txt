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

# cf_internal is a stub library for all of the internal calls within CF
# The "utilities" files are just some common helper code and hooks that are useful for
# many test cases, so they are also put in here (just a convenient spot to do so).
# All "stubs" files in here should be auto-generated using UtAssert generate_stubs.pl tool
# From the top-level directory of CF these can be regenerated with a command like:
#   ../../osal/ut_assert/scripts/generate_stubs.pl ./unit-test/stubs fsw/src/*.h
add_cfe_coverage_stubs(cf_internal
  utilities/cf_test_utils.c
  utilities/cf_test_alt_handler.c
  utilities/ut_utils_cfe_hooks.c

  stubs/cf_app_global.c
  stubs/cf_app_stubs.c
  stubs/cf_cfdp_handlers.c
  stubs/cf_cfdp_dispatch_stubs.c
  stubs/cf_cfdp_r_stubs.c
  stubs/cf_cfdp_s_stubs.c
  stubs/cf_cfdp_stubs.c
  stubs/cf_cfdp_sbintf_handlers.c
  stubs/cf_cfdp_sbintf_stubs.c
  stubs/cf_chunk_handlers.c
  stubs/cf_chunk_stubs.c
  stubs/cf_clist_handlers.c
  stubs/cf_clist_stubs.c
  stubs/cf_cmd_handlers.c
  stubs/cf_cmd_stubs.c
  stubs/cf_codec_handlers.c
  stubs/cf_codec_stubs.c
  stubs/cf_crc_stubs.c
  stubs/cf_timer_stubs.c
  stubs/cf_utils_handlers.c
  stubs/cf_utils_stubs.c
)

target_link_libraries(coverage-cf_internal-stubs core_api ut_assert)
target_include_directories(coverage-cf_internal-stubs PUBLIC utilities)

# Generate a dedicated "testrunner" executable for each test file
# Accomplish this by cycling through all the app's source files,
# there must be a *_tests file for each
foreach(SRCFILE ${APP_SRC_FILES})

    # Get the base sourcefile name as a module name without path or the
    # extension, this will be used as the base name of the unit test file.
    get_filename_component(UNIT_NAME "${SRCFILE}" NAME_WE)

    # Use the module name to make the tests' name by adding _tests to the end
    set(TESTS_NAME "${UNIT_NAME}_tests")

    # Get the tests' sourcefile name with unit test path and extension
    set(TESTS_SOURCE_FILE "${PROJECT_SOURCE_DIR}/unit-test/${TESTS_NAME}.c")

    # Create the coverage test executable
    # Note: This renders a target named "coverage-cf-${UNIT_NAME}-testrunner"
    add_cfe_coverage_test(cf "${UNIT_NAME}" "${TESTS_SOURCE_FILE}" "${CFS_CF_SOURCE_DIR}/${SRCFILE}")

    # CF test cases should be linked with stubs for other internal CF units
    add_cfe_coverage_dependency(cf "${UNIT_NAME}" cf_internal)

endforeach()
