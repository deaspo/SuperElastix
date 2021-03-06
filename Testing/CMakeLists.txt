#=========================================================================
#
#  Copyright Leiden University Medical Center, Erasmus University Medical 
#  Center and contributors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#=========================================================================

# ---------------------------------------------------------------------
# Download test data

# ExternalData module requires CMake version 2.8.11
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  message( FATAL_ERROR "Building tests require CMake 2.8.11 or newer." )
endif()

include( ExternalData )

list( APPEND ExternalData_URL_TEMPLATES
  "http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
  "https://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
)

# Scan the Data directory for content links on the form "[filename].[ext].md5". 
# The content links contains md5 hashes that are checked for consistensy  
# against files downloaded from servers in ExternalData_URL_TEMPLATES. These 
# files are placed in a corresponding location in the build directory.
file( GLOB_RECURSE SuperElastixDataContentLinks "*.md5" )
foreach( SuperElastixDataContentLink ${SuperElastixDataContentLinks} )
  string( REGEX REPLACE "\\.md5$" "" SuperElastixDataContentLink ${SuperElastixDataContentLink} )
  ExternalData_Expand_Arguments( SuperElastixData
    SuperElastixDataFileName
    DATA{${SuperElastixDataContentLink}}
  )
endforeach()

# Test data is downloaded when this target is built
ExternalData_Add_Target( SuperElastixData )

# ---------------------------------------------------------------------
# Find GoogleTest library

find_package( GTest REQUIRED )
include_directories( ${GTEST_INCLUDE_DIRS} )

set( TEST_LIBRARIES ${GTEST_BOTH_LIBRARIES} )

# ---------------------------------------------------------------------
# Build data manager

if( WIN32 )
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_INPUT_DATA_DIR ${SUPERELASTIX_INPUT_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_OUTPUT_DATA_DIR ${SUPERELASTIX_OUTPUT_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_BASELINE_DATA_DIR ${SUPERELASTIX_BASELINE_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_CONFIGURATION_DATA_DIR ${SUPERELASTIX_CONFIGURATION_DATA_DIR})
endif (WIN32)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/selxDataDirectories.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/selxDataDirectories.h
  ESCAPE_QUOTES
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
add_library( DataManager selxDataManager.cxx )


list( APPEND TEST_LIBRARIES DataManager )

# ---------------------------------------------------------------------
# Build test suite

if( ${BUILD_UNIT_TESTS} )
  add_subdirectory( Unit )
endif()

if( ${BUILD_INTEGRATION_TESTS} )
  add_subdirectory( Integration )
endif()

