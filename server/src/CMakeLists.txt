set(server_srcs Device.cpp DeviceHarness.cpp ServerUtil.cpp
ProtocolUtilities.cpp curlRequest.cpp CommunicationProtocol.cpp
CmdDevice.cpp  FlagMap.cpp)


set(exptserve_srcs ServerImpl.cpp ServerDescribeImpl.cpp 
    InvokeCapabilityImpl.cpp  FetchDataImpl.cpp 
    ExperimentalSetup.cpp ChunkStreamImpl.cpp 
    ServerFlagsImpl.cpp  BroadcastNotificationImpl.cpp FetchDataImpl.cpp exptserve.cpp
    StartSessionImpl.cpp KeepAliveSessionImpl.cpp
    CloseSessionImpl.cpp ListSessionsImpl.cpp
    exptserve_funcs.cpp
    pushbullet_api.cpp 
    )
    
    
if(BUILD_AUTH)
    list(APPEND server_srcs AuthHandler.cpp auth_utils.cpp SessionManager.cpp)
    list(APPEND exptserve_srcs AuthRequestImpl.cpp AuthRespondImpl.cpp)

endif()

add_subdirectory(streams)

add_library(foxtrot_server ${server_srcs} $<TARGET_OBJECTS:ft_server_streams>)
foxtrot_generate_export_header(${packname} foxtrot_server)
foxtrot_standard_include_dirs(foxtrot_server)
target_include_directories(foxtrot_server PUBLIC ${CURL_INCLUDE_DIR})
target_include_directories(foxtrot_server PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(foxtrot_server PUBLIC foxtrot::foxtrot_core ${CURL_LIBRARIES} RTTR::Core OpenSSL::SSL RapidJSON::RapidJSON)

if(BUILD_AUTH)
    target_link_libraries(foxtrot_server PUBLIC libsodium::libsodium)
endif()

add_executable(exptserve ${exptserve_srcs})
target_include_directories(exptserve PRIVATE ${CMAKE_SOURCE_DIR}/include/exptserve/)
foxtrot_standard_include_dirs(exptserve)

target_link_libraries(exptserve PUBLIC foxtrot::foxtrot_core ${dl} RTTR::Core)
target_link_libraries(exptserve PUBLIC foxtrot_server Boost::filesystem Boost::program_options)

if(BUILD_AUTH)
    add_executable(ftauth ftauth.cpp)
    target_link_libraries(ftauth Boost::program_options libsodium::libsodium foxtrot::foxtrot_core
    foxtrot_server)
    target_include_directories(ftauth PRIVATE include)
endif()




