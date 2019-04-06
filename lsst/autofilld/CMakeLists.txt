add_executable(autofilld autofilld.cpp autofill_logger.cpp autofill_fill_logic.cpp)


target_link_libraries(autofilld foxtrotclient ${Boost_FILESYSTEM_LIBRARY}
${Boost_DATE_TIME_LIBRARY})

