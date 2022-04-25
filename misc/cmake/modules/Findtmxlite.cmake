include(FindPackageHandleStandardArgs)

# Search for the header file
find_path(TMXLITE_INCLUDE_DIR NAMES tmxlite/Config.hpp)

# Search for the library
find_library(TMXLITE_LIBRARIES NAMES tmxlite)

# Did we find everything we need?
FIND_PACKAGE_HANDLE_STANDARD_ARGS(tmxlite DEFAULT_MSG TMXLITE_LIBRARIES TMXLITE_INCLUDE_DIR) 
