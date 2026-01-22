###########################################################
#
# CF App mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Add stand alone documentation
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/docs/dox_src ${MISSION_BINARY_DIR}/docs/cf-usersguide)

# The list of header files that control the CF configuration
set(CF_MISSION_CONFIG_FILE_LIST
  cf_extern_typedefs.h
  cf_fcncode_values.h
  cf_interface_cfg_values.h
  cf_mission_cfg.h
  cf_msgdefs.h
  cf_msg.h
  cf_msgstruct.h
  cf_tbldefs.h
  cf_tbl.h
  cf_tblstruct.h
  cf_topicid_values.h
)

generate_configfile_set(${CF_MISSION_CONFIG_FILE_LIST})