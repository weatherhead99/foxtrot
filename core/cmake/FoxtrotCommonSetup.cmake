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
    
    configure_package_config_file(${infile} installtree/${outfname}.cmake
                        INSTALL_DESTINATION ${destdir}
                        PATH_VARS ${path_vars})
    
    configure_package_config_file(${infile} ${outfname}.cmake
                        INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
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


macro(foxtrot_writeout_build_directory filename)
    message("writing out location of build directory")
    file(WRITE ${filename} "${CMAKE_BINARY_DIR}\n")
endmacro()
