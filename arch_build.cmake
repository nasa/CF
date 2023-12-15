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

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(CF_CFGFILE ${CF_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${CF_CFGFILE}" NAME_WE)
  if (DEFINED CF_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${CF_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${CF_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${CF_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
