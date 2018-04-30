add_executable(watchdog  watchdog.cpp WatchDogEvent.cpp WatchDogHeater.cpp)
target_include_directories(watchdog PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/client)
target_include_directories(watchdog PRIVATE ${CMAKE_BINARY_DIR}/server)
target_link_libraries(watchdog foxtrot foxtrotclient)
