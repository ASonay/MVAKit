cmake_minimum_required( VERSION 3.1 )

# Silence some warnings on macOS with new CMake versions.
if( NOT ${CMAKE_VERSION} VERSION_LESS 3.9 )
   cmake_policy( SET CMP0068 NEW )
endif()

# Set the project's name and version.
project( TMVATool )

# Enable using CTest in the project.
include( CTest )

# Set up the "C++ version" to use.
set( CMAKE_CXX_STANDARD_REQUIRED 11 CACHE STRING
   "Minimum C++ standard required for the build" )
set( CMAKE_CXX_STANDARD 17 CACHE STRING
   "C++ standard to use for the build" )
set( CMAKE_CXX_EXTENSIONS FALSE CACHE BOOL
   "(Dis)allow using compiler extensions" )

# Specify the install locations for libraries and binaries.
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib )  # Needed by ROOT_GENERATE_DICTIONARY()

# Set the warning flag(s) to use.
set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic -O3 -g -fPIC" )


# ------------------------------------------------
# Dependencies and sub-libraries
# ------------------------------------------------

# Add ROOT system directory and require ROOT.
find_package( ROOT 6.10.06 REQUIRED COMPONENTS Core MathCore Tree TMVA )

# ------------------------------------------------
# TMVATool library
# ------------------------------------------------

set( lib_headers
  inc/ReadTree.hpp
  inc/TMVAConf.hpp
  inc/TMVARead.hpp)
set( lib_sources
  src/ReadTree.cpp
  src/TMVAConf.cpp
  src/TMVARead.cpp)

add_library( TMVATool SHARED ${lib_headers} ${lib_sources} )
target_include_directories( TMVATool
  PUBLIC ${ROOT_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}> $<INSTALL_INTERFACE:> )
target_link_libraries( TMVATool ${ROOT_LIBRARIES} )
set_property( TARGET TMVATool
  PROPERTY PUBLIC_HEADER ${lib_headers} )
target_include_directories(TMVATool PUBLIC ${CMAKE_CURRENT_LIST_DIR} )

# Install the libraries.
install( TARGETS TMVATool
  EXPORT TMVAToolTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/TMVATool )

# ------------------------------------------------
# TMVATool executables
# ------------------------------------------------

# Helper macro for building the project's executables.
macro( TMVATool_add_executable name )
  add_executable( ${name} ${ARGN} )
  target_include_directories( ${name} PUBLIC ${ROOT_INCLUDE_DIRS} )
  target_link_libraries( ${name} TMVATool ${ROOT_LIBRARIES} )
  install( TARGETS ${name}
    EXPORT TMVAToolTargets
    RUNTIME DESTINATION bin )
endmacro( TMVATool_add_executable )

TMVATool_add_executable( tmva_train util/tmva_train.cxx )
TMVATool_add_executable( tmva_eval util/tmva_eval.cxx )

add_definitions( -DTMVATOOL_HOME=${CMAKE_CURRENT_SOURCE_DIR} )
