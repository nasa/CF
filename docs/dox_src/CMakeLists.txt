########################################################
#
# CMake Recipe to build doxygen documentation
#
########################################################

# This CMake script currently defines a top-level target
# to build the stand alone documentation.  This may be invoked either
# from the main CMakeLists.txt as a subdirectory (useful in the
# case of a self-contained/standalone build) or by a separate script
# (useful if integrating into a larger project with a separate doc build)

project(CF_DOCS NONE)

# generate the configuration files
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cf-common.doxyfile.in
        ${CMAKE_BINARY_DIR}/docs/cf-common.doxyfile
        @ONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cf-usersguide.doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/cf-usersguide.doxyfile
        @ONLY
)

add_custom_target(cf-usersguide-html
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/cf-usersguide.doxyfile
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# This requires some of the intermediate files from the CFE doc build to be
# in place prior to running doxygen for CF
add_dependencies(cf-usersguide-html doc-prebuild)

add_custom_target(cf-usersguide
    COMMAND echo "CF UsersGuide: file://${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
    DEPENDS cf-usersguide-html
)
