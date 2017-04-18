add_device_lib(BME280 BME280.cpp)

add_executable(bme280_play bme280_play.cpp)
target_link_libraries(bme280_play BME280 foxtrot)
