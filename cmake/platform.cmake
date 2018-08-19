
#enable c++ standard exception handling on windows
#set dynamic runtime
if(MSVC)
set(CMAKE_CXX_FLAGS "/EHsc")
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "ON")

endif()