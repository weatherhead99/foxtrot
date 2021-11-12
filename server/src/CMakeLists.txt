set(server_srcs Device.cpp DeviceHarness.cpp ServerUtil.cpp
ProtocolUtilities.cpp curlRequest.cpp CommunicationProtocol.cpp
CmdDevice.cpp  FlagMap.cpp auth_utils.cpp DeviceLocks.cpp)


set(exptserve_srcs ServerImpl.cpp ServerDescribeImpl.cpp 
    InvokeCapabilityImpl.cpp  FetchDataImpl.cpp 
    ExperimentalSetup.cpp ChunkStreamImpl.cpp 
    ServerFlagsImpl.cpp  BroadcastNotificationImpl.cpp FetchDataImpl.cpp exptserve.cpp
    StartSessionImpl.cpp KeepAliveSessionImpl.cpp
    CloseSessionImpl.cpp ListSessionsImpl.cpp
    exptserve_funcs.cpp pushbullet_api.cpp
    AuthHandler.cpp SessionManager.cpp
    AuthRequestImpl.cpp AuthRespondImpl.cpp
    )
    

add_subdirectory(streams)

add_library(foxtrot_server ${server_srcs} $<TARGET_OBJECTS:ft_server_streams>)
foxtrot_generate_export_header(${packname} foxtrot_server)
target_include_directories(foxtrot_server PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(foxtrot_server PUBLIC foxtrot::foxtrot_core  RTTR::Core OpenSSL::SSL RapidJSON::RapidJSON CURL::libcurl libsodium::libsodium)


add_executable(exptserve ${exptserve_srcs})
target_include_directories(exptserve PRIVATE ${CMAKE_SOURCE_DIR}/include/exptserve/)

target_link_libraries(exptserve PUBLIC foxtrot::foxtrot_core ${dl} RTTR::Core foxtrot_server Boost::filesystem Boost::program_options)

add_executable(ftauth ftauth.cpp)
target_link_libraries(ftauth Boost::program_options libsodium::libsodium foxtrot::foxtrot_core  foxtrot_server)
target_include_directories(ftauth PRIVATE include)



