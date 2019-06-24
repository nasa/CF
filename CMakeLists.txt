cmake_minimum_required(VERSION 2.6.4)
project(CFS_CF C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src/PRI)
include_directories(fsw/src/PUB)
include_directories(fsw/src)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(cf ${APP_SRC_FILES})
