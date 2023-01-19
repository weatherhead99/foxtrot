import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.3]@weatherill/stable")

class FoxtrotCoreConan(ftbase.FoxtrotCppPackage):
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    exports_sources = "CMakeLists.txt", "src/*.cpp", "src/*.cc", \
                      "include/foxtrot/*.h", "include/foxtrot/*.hpp", \
                      "include/foxtrot/*.hh", \
                      "cmake/*.cmake", "cmake/*.cmake.in", \
                      "proto/*.proto", "share/*.in", "devprogs/*" ,\
                      "proto/CMakeLists.txt"

    requires = ("boost/1.80.0",
                "grpc/1.50.1",
                "rttr/0.9.6")

    default_options = {"boost:shared" : True,
                       "OpenSSL:shared": True,
                       "protobuf:with_zlib": True,
                       "protobuf:shared" : True,
                       "rttr:shared" : True,
                       "rttr:with_rtti" : True,
                       "grpc:cpp_plugin" : True}

    src_folder = "core"


    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotCore")
