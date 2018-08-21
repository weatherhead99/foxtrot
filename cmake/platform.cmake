
#enable c++ standard exception handling on windows
#set dynamic runtime
if(MSVC)
	set(CMAKE_CXX_FLAGS "/EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "/MD")
	set(CMAKE_CXX_FLAGS_DEBUG "/MDd")

	#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "ON")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	#64 bit build
		set(PROJECTNAME_ARCHITECTURE "x64")

	endif()

	#sets which winapi versions are needed, including e.g. killing
	#a boost_log_abi error
	add_definitions(-D_WIN32_WINNT=0x601)
	set(EXTRA_WINLIBS "ws2_32")
	
endif()