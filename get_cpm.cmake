# This script is part of CPM.cmake under the following license
#
# SPDX-License-Identifier: MIT
#
#

set(CPM_DOWNLOAD_VERSION 0.42.0)
set(CPM_HASH_SUM "2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a")
set(CPM_FETCH_STRING "Need to fetch package")

if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

file(DOWNLOAD
     https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
     ${CPM_DOWNLOAD_LOCATION} EXPECTED_HASH SHA256=${CPM_HASH_SUM}
)

include(${CPM_DOWNLOAD_LOCATION})

if(NOT COMMAND _cpm_declare_fetch)
    macro(cpm_declare_fetch)
        # DO NOT REMOVE: used as cache indicator for runtime-ci
        message(STATUS "${CPM_FETCH_STRING}: ${ARGV0} ${ARGV1}")

        _cpm_declare_fetch(${ARGV})
    endmacro()
endif()
get_filename_component(CPM_SOURCE_CACHE ${CPM_SOURCE_CACHE} ABSOLUTE)

# Override to handle internal gitlab repositories by supplying an access token as https attribute
if(DEFINED ENV{CI_JOB_TOKEN} AND NOT COMMAND _cpm_declare_fetch)
    function(cpm_declare_fetch package versiong info)
        set(new_argv "${package};${versiong};${info}")
        foreach(arg ${argv})
            string(REPLACE "git@code.siemens.com:"
                           "https://gitlab-ci-token:$ENV{CI_JOB_TOKEN}@code.siemens.com/" new_arg
                           "${arg}"
            )
            set(new_argv ${new_argv};${new_arg})
        endforeach()
        _cpm_declare_fetch(${new_argv})
    endfunction()
endif()
