function(get_proto_version proto_file outvar)
    file(READ ${proto_file} PROTO_TXT)
    string(FIND "${PROTO_TXT}" "FOXTROT_PROTO_VERSION" matchres)
    if(${matchres} EQUAL -1)
        message(WARNING "couldn't get version from file: ${proto_file}")
    else()
        message(STATUS "FOXTROT_PROTO_VERSION: ${matchres}")
    
    endif()
    

    set(outvar ${matchres})
endfunction(get_proto_version)


