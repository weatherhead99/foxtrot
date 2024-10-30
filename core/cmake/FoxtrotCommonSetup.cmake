#cmake policy settings in the modern era


#set shared library build by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared libraries")
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "build type")
include(GNUInstallDirs)

function(foxtrot_generate_pkg_config_find_module FT_PACKNAME PC_MODULE_NAME)
  cmake_parse_arguments(FT_PC "" "" TARGET_ALIASES ${ARGN})

  message(STATUS "generating find module for pkg-config module name: ${PC_MODULE_NAME}")
  find_file(TEMPLATE_IN findpkgconfiglibrarygeneric.cmake.in ${CMAKE_CURRENT_FUNCTION_LIST_DIR})

  foxtrot_packname_to_cmake_package_name(${FT_PACKNAME} CMAKE_PACKNAME)
  
  
  set(CMAKE_DEST ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/${CMAKE_PACKNAME})
  
  set(outfname Find${PC_MODULE_NAME}.cmake)
  #NOTE: always install to top level binary dir, for compatibility with
  #subdirectory find package generation (i.e. currently aravis_camera)
  configure_file(${TEMPLATE_IN} ${CMAKE_BINARY_DIR}/${outfname} @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/${outfname}
    DESTINATION ${CMAKE_DEST} COMPONENT devel)

  
endfunction()


function(foxtrot_build_setup_file fname)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "TARGET_NAME" "")

  
  if(DEFINED arg_TARGET_NAME)
    set(tgtname ${arg_TARGET_NAME})
  else()
    message(DEBUG "no target name passed, using filename...")
    get_filename_component(tgtname ${fname} NAME_WE)
   endif()

  message(DEBUG "target name: ${tgtname}")

  add_library(${tgtname} MODULE ${fname})
  target_link_libraries(${tgtname} PUBLIC foxtrot::foxtrot_core foxtrot::foxtrot_protocols foxtrot::foxtrot_devices)
  set_target_properties(${tgtname} PROPERTIES PREFIX "")

  install(TARGETS ${tgtname}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ft_setups/)
  

endfunction()


macro(foxtrot_generate_export_header packname tgtname)
  include(GenerateExportHeader)
  generate_export_header(${tgtname}
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_${packname}_export.h)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_${packname}_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
    COMPONENT devel)
  
endmacro()


#NOTE: this one I don't think is used ANYWHERE anymore
function(foxtrot_standard_include_dirs tgt)
  target_include_directories(${tgt} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  target_include_directories(${tgt} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  install(DIRECTORY include/foxtrot
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT devel)
endfunction()



function(foxtrot_standard_tgt_install tgtname packname)

  foxtrot_packname_to_cmake_export_name(packname)
  install(TARGETS ${tgtname}
    EXPORT ${packname}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endfunction()

	

function(foxtrot_setup_cmake_package packname target_name sourcedir_name deps_script)
#  message(STATUS "include path: ${CMAKE_INCLUDE_PATH}")
#  message(STATUS "module path: ${CMAKE_MODULE_PATH}")
  message(STATUS "source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
  find_file(package_infile NAMES "foxtrotGenericConfig.cmake.in"
    HINTS ${CMAKE_MODULE_PATH} REQUIRED NO_CACHE)

  message(STATUS "package infile: ${package_infile}")
  set(outfname ${packname}Config.cmake)
  set(CMAKE_DEST ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/${packname}/)

  set(DEPS_SCRIPT ${deps_script})
  set(PACKNAME ${packname})
  set(TARGETNAME ${target_name})
  set(SOURCEDIR_NAME ${sourcedir_name})
  set(SOURCEDIR_PATH ${CMAKE_CURRENT_SOURCE_DIR})

  include(CMakePackageConfigHelpers)
  
  configure_package_config_file(${package_infile} ${CMAKE_CURRENT_BINARY_DIR}/${outfname}
    INSTALL_DESTINATION ${CMAKE_DEST}
  PATH_VARS CMAKE_DEST SOURCEDIR_PATH)

  set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/${packname})
  install(EXPORT ${target_name}
    DESTINATION ${ft_cmakedir}
    COMPONENT devel
    NAMESPACE foxtrot::)
  


  export(EXPORT ${target_name} FILE ${CMAKE_CURRENT_BINARY_DIR}/${packname}exports.cmake NAMESPACE foxtrot::)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${outfname}
    DESTINATION ${CMAKE_DEST} COMPONENT devel)

  export(PACKAGE ${packname})

  include(CPack)
  set(CPACK_PACKAGE_NAME "foxtrot_${sourcedir_name}")
  set(CPACK_PACKAGE_VENDOR "OPMD")
  set(CPACK_PACKAGE_CONTACT "Dan Weatherill <daniel.weatherill@physics.ox.ac.uk>")
  

endfunction()


function(foxtrot_packname_to_cmake_package_name packname out_var)
  string(SUBSTRING ${packname} 0 1 FIRST_LETTER)
  string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
  string(SUBSTRING ${packname} 1 -1 REST)
  string(CONCAT name_capitalized ${FIRST_LETTER} ${REST}) 
  set(${out_var} "foxtrot${name_capitalized}")
  return(PROPAGATE ${out_var})
endfunction()

function(foxtrot_packname_to_cmake_export_name packname out_var)
  set(${out_var} "foxtrot_${packname}")
  return(PROPAGATE ${out_var})
endfunction()


function(foxtrot_standard_setup_cmake_package sourcedir_name deps_script)

  foxtrot_packname_to_cmake_package_name(${sourcedir_name} PACKNAME)
  set(TARGETNAME "foxtrot_${sourcedir_name}")

  message(STATUS "packagename: ${PACKNAME}")
  message(STATUS "target name: ${TARGETNAME}")
  

  foxtrot_setup_cmake_package(${PACKNAME} ${TARGETNAME} ${sourcedir_name}  ${deps_script})
endfunction()

