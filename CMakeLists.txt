cmake_minimum_required( VERSION 3.1 )

# Silence some warnings on macOS with new CMake versions.
if( NOT ${CMAKE_VERSION} VERSION_LESS 3.9 )
   cmake_policy( SET CMP0068 NEW )
endif()

# Set the project's name and version.
project( MVAKit )

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
find_package( ROOT 6.10.06 REQUIRED COMPONENTS Core MathCore Tree TMVA PyMVA)

link_libraries(stdc++fs)

# ------------------------------------------------
# MVAKit library
# ------------------------------------------------

set( lib_headers
  MVAKit/Common.hpp
  MVAKit/ReadTree.hpp
  MVAKit/Configuration.hpp
  MVAKit/MVAKit.hpp
  MVAKit/EvalTMVA.hpp
  MVAKit/Layer.hpp
  MVAKit/Activation.hpp
  MVAKit/NeuralNetwork.hpp
  MVAKit/Transformers.hpp )
set( lib_sources
  src/External.cpp
  src/Common.cpp
  src/ReadTree.cpp
  src/Configuration.cpp
  src/MVAKit.cpp
  src/EvalTMVA.cpp
  src/Layer.cpp
  src/Activation.cpp
  src/NeuralNetwork.cpp
  src/Transformers.cpp )

add_library( MVAKit SHARED ${lib_headers} ${lib_sources} )
target_include_directories( MVAKit
  PUBLIC ${ROOT_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}> $<INSTALL_INTERFACE:>)
target_link_libraries( MVAKit ${ROOT_LIBRARIES} )
set_property( TARGET MVAKit
  PROPERTY PUBLIC_HEADER ${lib_headers} )
target_include_directories( MVAKit PUBLIC ${CMAKE_CURRENT_LIST_DIR} )

# Install the libraries.
install( TARGETS MVAKit
  EXPORT MVAKitTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/MVAKit )

# ------------------------------------------------
# MVAKit executables
# ------------------------------------------------

# Helper macro for building the project's executables.
macro( MVAKit_add_executable name )
  add_executable( ${name} ${ARGN} )
  target_include_directories( ${name} PUBLIC ${ROOT_INCLUDE_DIRS} )
  target_link_libraries( ${name} MVAKit ${ROOT_LIBRARIES} )
  install( TARGETS ${name}
    EXPORT MVAKitTargets
    RUNTIME DESTINATION bin )
endmacro( MVAKit_add_executable )

MVAKit_add_executable( tmva_train util/tmva_train.cxx )
MVAKit_add_executable( create_ntuple util/create_ntuple.cxx )
MVAKit_add_executable( clone_ntuple util/clone_ntuple.cxx )

add_definitions( -DTMVATOOL_HOME=${CMAKE_CURRENT_SOURCE_DIR} )
