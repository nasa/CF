###########################################################
#
# CF App platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the CF configuration
set(CF_PLATFORM_CONFIG_FILE_LIST
  cf_internal_cfg.h
  cf_msgids.h
  cf_platform_cfg.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, msgids come generated from the EDS tool
  set(CF_CFGFILE_SRC_cf_msgids "${CMAKE_CURRENT_LIST_DIR}/config/cf_eds_msg_topicids.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(CF_CFGFILE ${CF_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${CF_CFGFILE}" NAME_WE)
  if (DEFINED CF_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE "${CF_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE "${CMAKE_CURRENT_LIST_DIR}/config/default_${CF_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${CF_CFGFILE}"
    FALLBACK_FILE       ${DEFAULT_SOURCE}
  )
endforeach()
