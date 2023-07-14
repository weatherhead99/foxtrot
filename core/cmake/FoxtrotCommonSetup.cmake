#setup conan build if necessary
# message(STATUS "checking for conan build...")
# if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#     message(WARNING "conanbuildinfo detected, building using
#     conan dependencies")
#     include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#     set(FOXTROT_CORE_CONAN_BUILD TRUE)
#     conan_basic_setup()
# endif()


#set shared library build by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared libraries")
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "build type")


macro(foxtrot_generate_export_header packname tgtname)
    include(GenerateExportHeader)
    generate_export_header(${tgtname}
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_${packname}_export.h)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_${packname}_export.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
            COMPONENT devel)
endmacro()

function(foxtrot_standard_include_dirs tgt)
    target_include_directories(${tgt} PUBLIC
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_include_directories(${tgt} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    install(DIRECTORY include/foxtrot
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT devel)
endfunction()

function(foxtrot_create_package_config infile destdir path_vars)
    include(CMakePackageConfigHelpers)
    get_filename_component(outfname_withpath ${infile} NAME_WE)
    get_filename_component(outfname ${outfname_withpath} NAME)
    
    configure_package_config_file(${infile} ${outfname}.cmake
                        INSTALL_DESTINATION ${destdir}
                        PATH_VARS ${path_vars}
                        INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/installtree/${outfname}.cmake
            DESTINATION ${destdir}
            COMPONENT devel)
endfunction()

function(foxtrot_add_to_package_registry exportname packagename)
    message("writing temporary CMake export file to build tree")
    export(EXPORT ${exportname} FILE ${CMAKE_CURRENT_BINARY_DIR}/${packagename}exports.cmake
      NAMESPACE foxtrot::)
    if(FT_EXPORT_TO_PACKAGE_REGISTRY)
        message("exporting to package registry")
        export(PACKAGE ${packagename})
    endif()
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

  export(EXPORT ${target_name} FILE ${CMAKE_CURRENT_BINARY_DIR}/${packname}exports.cmake NAMESPACE foxtrot::)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${outfname}
    DESTINATION ${CMAKE_DEST} COMPONENT devel)

endfunction()


function(foxtrot_standard_setup_cmake_package sourcedir_name deps_script)
  string(SUBSTRING ${sourcedir_name} 0 1  FIRST_LETTER) 
  string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)

  string(SUBSTRING ${sourcedir_name} 1 -1 REST)
  string(CONCAT name_capitalized ${FIRST_LETTER} ${REST})
  
  set(PACKNAME "foxtrot${name_capitalized}")
  set(TARGETNAME "foxtrot_${sourcedir_name}")

  message(STATUS "packagename: ${PACKNAME}")
  message(STATUS "target name: ${TARGETNAME}")
  

  foxtrot_setup_cmake_package(${PACKNAME} ${TARGETNAME} ${sourcedir_name}  ${deps_script})
endfunction()




# function(foxtrot_setup_cmake_package infile packname target_name)
#   include(CMakePackageConfigHelpers)

#   #work out where the file will go (just the build directory)
#   get_filename_component(outfname_withpath ${infile} NAME_WE)
#   get_filename_component(outfname ${outfname_withpath} NAME)

#   set(CMAKE_DEST ${CMAKE_INSTALL_LIBDIR}/cmake/${packname}/)  
#   configure_package_config_file(${infile} ${CMAKE_CURRENT_BINARY_DIR}/${outfname}.cmake
#     INSTALL_DESTINATION ${CMAKE_DEST}
#     )
  
#   export(EXPORT ${target_name} FILE ${CMAKE_CURRENT_BINARY_DIR}/${packname}exports.cmake NAMESPACE foxtrot::)

#   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${outfname}.cmake
#     DESTINATION ${CMAKE_DEST} COMPONENT devel)
  
# endfunction()



macro(foxtrot_writeout_build_directory filename)
    message("writing out location of build directory")
    file(WRITE ${filename} "${CMAKE_BINARY_DIR}\n")
endmacro()
