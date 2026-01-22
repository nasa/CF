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
  cf_internal_cfg_values.h
  cf_msgid_values.h
  cf_msgids.h
  cf_platform_cfg.h
)

generate_configfile_set(${CF_PLATFORM_CONFIG_FILE_LIST})
