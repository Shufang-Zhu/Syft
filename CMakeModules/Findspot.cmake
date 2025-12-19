# Try to find SPOT headers and libraries.
#
# Usage of this module as follows:
#
# find_package(SPOT)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# SPOT_ROOT Set this variable to the root installation of
# libcudd if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# SPOT_FOUND System has SPOT libraries and headers
# SPOT_LIBRARIES The SPOT library
# SPOT_INCLUDE_DIRS The location of SPOT headers

# Get hint from environment variable (if any)
if(NOT SPOT_ROOT AND DEFINED ENV{SPOT_ROOT})
    set(SPOT_ROOT "$ENV{SPOT_ROOT}" CACHE PATH "SPOT base directory location (optional, used for nonstandard installation paths)")
    mark_as_advanced(SPOT_ROOT)
endif()

#set(SPOT_ROOT "$ENV{HOME}/install")
set(SPOT_ROOT "/usr/local")
# Search path for nonstandard locations
if(SPOT_ROOT)
    set(SPOT_INCLUDE_PATH PATHS "${SPOT_ROOT}/include" NO_DEFAULT_PATH)
    set(SPOT_LIBRARY_PATH PATHS "${SPOT_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(SPOT_INCLUDE_DIRS NAMES spot HINTS ${SPOT_INCLUDE_PATH})
find_library(SPOT_LIBRARIES NAMES spot HINTS ${SPOT_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(spot DEFAULT_MSG SPOT_LIBRARIES SPOT_INCLUDE_DIRS)

mark_as_advanced(SPOT_ROOT SPOT_LIBRARIES SPOT_INCLUDE_DIRS)
