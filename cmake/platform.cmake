
#enable c++ standard exception handling on windows
if(MSVC)
set(CMAKE_CXX_FLAGS "/EHsc")
endif()