# Try to find MONA headers and libraries.
#
# Usage of this module as follows:
#
# find_package(MONA)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# MONA_ROOT Set this variable to the root installation of
# libmona if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# MONA_FOUND System has MONA libraries and headers
# MONA_LIBRARIES The MONA library
# MONA_INCLUDE_DIRS The location of MONA headers

# Get hint from environment variable (if any)
if(NOT MONA_ROOT AND DEFINED ENV{MONA_ROOT})
    set(MONA_ROOT "$ENV{MONA_ROOT}" CACHE PATH "MONA base directory location (optional, used for nonstandard installation paths)")
    mark_as_advanced(MONA_ROOT)
endif()

#set(MONA_ROOT "$ENV{HOME}/install")
set(MONA_ROOT "/usr/local")
# Search path for nonstandard locations
if(MONA_ROOT)
    set(MONA_INCLUDE_PATH PATHS "${MONA_ROOT}/include/mona" NO_DEFAULT_PATH)
    set(MONA_LIBRARY_PATH PATHS "${MONA_ROOT}/lib" NO_DEFAULT_PATH)
endif()

# Search for mona_mem
find_path(MONA_MEM_INCLUDE_DIRS NAMES mem.h HINTS ${MONA_INCLUDE_PATH})
find_library(MONA_MEM_LIBRARIES NAMES libmonamem.a HINTS ${MONA_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mona DEFAULT_MSG MONA_MEM_LIBRARIES MONA_MEM_INCLUDE_DIRS)

mark_as_advanced(MONA_ROOT MONA_MEM_LIBRARIES MONA_MEM_INCLUDE_DIRS)

# Search for mona_bdd
find_path(MONA_BDD_INCLUDE_DIRS NAMES bdd.h HINTS ${MONA_INCLUDE_PATH})
find_library(MONA_BDD_LIBRARIES NAMES libmonabdd.a HINTS ${MONA_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mona DEFAULT_MSG MONA_BDD_LIBRARIES MONA_BDD_INCLUDE_DIRS)

mark_as_advanced(MONA_ROOT MONA_BDD_LIBRARIES MONA_BDD_INCLUDE_DIRS)

# Search for mona_dfa
find_path(MONA_DFA_INCLUDE_DIRS NAMES dfa.h HINTS ${MONA_INCLUDE_PATH})
find_library(MONA_DFA_LIBRARIES NAMES libmonadfa.a HINTS ${MONA_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(mona DEFAULT_MSG MONA_DFA_LIBRARIES MONA_DFA_INCLUDE_DIRS)

mark_as_advanced(MONA_ROOT MONA_DFA_LIBRARIES MONA_DFA_INCLUDE_DIRS)

