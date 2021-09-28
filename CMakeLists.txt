cmake_minimum_required(VERSION 2.6.4)
project(CFS_CF_APP C)

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${CFS_IO_LIB_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(cf_app ${APP_SRC_FILES})

if (COMMAND add_cfe_tables)
    aux_source_directory(fsw/tables APP_TBL_FILES)
    add_cfe_tables(cf_app ${APP_TBL_FILES})
endif(COMMAND add_cfe_tables)

add_definitions("-D_DEFAULT_SOURCE")

add_definitions(-D_DEFAULT_SOURCE=1)
add_definitions(-D_EL -DENDIAN=_EL -DSOFTWARE_BIG_BIT_ORDER)
if (ENABLE_UNIT_TESTS)
  set(CMAKE_C_FLAGS "-O0")
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
