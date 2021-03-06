# $Id: $
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#--------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package ( AlignDet MAJOR 0 MINOR 0 PATCH 1
  USES           [ROOT   REQUIRED COMPONENTS Geom GenVector MathCore] 
                 [DD4hep REQUIRED COMPONENTS DDCore]
  OPTIONAL       XERCESC
  INCLUDE_DIRS   include
  )

dd4hep_add_plugin( AlignDet SOURCES src/*.cpp  )
dd4hep_install_dir( compact DESTINATION examples/AlignDet )
dd4hep_configure_scripts( AlignDet DEFAULT_SETUP WITH_TESTS)

#
#---Testing: Load ALEPH TPC geometry --------------------------------------
dd4hep_add_test_reg( test_DetAlign_AlephTPC_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC.xml
             -destroy -no-interpreter -plugin DD4hep_GlobalAlignmentInstall
  REGEX_PASS "189 nodes/ 23 volume UID's in LCDD Geometry" )
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( test_DetAlign_Global_AlephTPC_align
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hepXMLLoader file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC_alignment.xml BUILD_DEFAULT
  REGEX_PASS "Successfully parsed XML: AlephTPC_alignment.xml")
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( test_DetAlign_Global_AlephTPC_reset
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hepXMLLoader file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC_alignment.xml
             -plugin DD4hepXMLLoader file:${CMAKE_CURRENT_SOURCE_DIR}/compact/AlephTPC_reset.xml
  REGEX_PASS "Successfully parsed XML: AlephTPC_reset.xml" )
