add_device_lib(NAME BME280 SRCS BME280.cpp)

add_executable(bme280_play bme280_play.cpp)
target_link_libraries(bme280_play BME280 foxtrot)
