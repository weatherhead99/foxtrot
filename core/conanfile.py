import os
from conans import python_requires
from conan.tools.cmake import cmake_layout, CMake, CMakeDeps
from conans.tools import RunEnvironment, environment_append

ftbase = python_requires("FoxtrotBuildUtils/[^0.4]@weatherill/stable")

class FoxtrotCoreConan(ftbase.FoxtrotCppPackage):
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    exports_sources = "CMakeLists.txt", "src/*.cpp", "src/*.cc", \
                      "include/foxtrot/*.h", "include/foxtrot/*.hpp", \
                      "include/foxtrot/*.hh", \
                      "cmake/*.cmake", "cmake/*.cmake.in", \
                      "proto/*.proto", "share/*.in", "devprogs/*" ,\
                      "proto/CMakeLists.txt"

    requires = ("boost/[^1.80.0]",
                "grpc/[^1.50.1]",
                "rttr/[^0.9.6]")

    default_options = {"boost:shared" : True,
                       "OpenSSL:shared": True,
                       "protobuf:with_zlib": True,
                       "protobuf:shared" : True,
                       "rttr:shared" : True,
                       "rttr:with_rtti" : True,
                       "grpc:cpp_plugin" : True}

    src_folder = "core"
    cmakeName = "foxtrotCore"

    def package_info(self):
        super().package_info()
        #no need for CMakeDeps to generate CMake files,
        #we do that ourselves
        self.cpp_info.set_property("cmake_find_mode", "none")

    def layout(self):
        super().layout()
        self.cpp.source.includedirs.append("include")
