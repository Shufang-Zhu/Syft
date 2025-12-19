# Try to find BUDDY headers and libraries.
#
# Usage of this module as follows:
#
# find_package(BUDDY)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# BUDDY_ROOT Set this variable to the root installation of
# libcudd if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# BUDDY_FOUND System has BUDDY libraries and headers
# BUDDY_LIBRARIES The BUDDY library
# BUDDY_INCLUDE_DIRS The location of BUDDY headers

# Get hint from environment variable (if any)
if(NOT BUDDY_ROOT AND DEFINED ENV{BUDDY_ROOT})
    set(BUDDY_ROOT "$ENV{BUDDY_ROOT}" CACHE PATH "BUDDY base directory location (optional, used for nonstandard installation paths)")
    mark_as_advanced(BUDDY_ROOT)
endif()

#set(BUDDY_ROOT "$ENV{HOME}/install")
set(BUDDY_ROOT "/usr/local")
# Search path for nonstandard locations
if(BUDDY_ROOT)
    set(BUDDY_INCLUDE_PATH PATHS "${BUDDY_ROOT}/include" NO_DEFAULT_PATH)
    set(BUDDY_LIBRARY_PATH PATHS "${BUDDY_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(BUDDY_INCLUDE_DIRS NAMES bddx.h HINTS ${BUDDY_INCLUDE_PATH})
find_library(BUDDY_LIBRARIES NAMES bddx HINTS ${BUDDY_LIBRARY_PATH})

message(STATUS "BUDDY_INCLUDE_PATH: ${BUDDY_INCLUDE_PATH}")
message(STATUS "BUDDY_LIBRARY_PATH: ${BUDDY_LIBRARY_PATH}")
message(STATUS "BUDDY_INCLUDE_DIRS: ${BUDDY_INCLUDE_DIRS}")
message(STATUS "BUDDY_LIBRARIES: ${BUDDY_LIBRARIES}")


include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(buddy DEFAULT_MSG BUDDY_LIBRARIES BUDDY_INCLUDE_DIRS)

mark_as_advanced(BUDDY_ROOT BUDDY_LIBRARIES BUDDY_INCLUDE_DIRS)
