import os
from conan import ConanFile
from conan.tools.cmake import CMake
from conan.tools.env import VirtualRunEnv

#ftbase = python_requires("foxtrotbuildutils/[^0.4]@weatherill/stable")

class FoxtrotCoreConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
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

    
    default_options = {"boost/*:shared" : True,
                       "OpenSSL/*:shared": True,
                       "protobuf/*:with_zlib": True,
                       "protobuf/*:shared" : True,
                       "rttr/*:shared" : True,
                       "rttr/*:with_rtti" : True,
                       "grpc/*:cpp_plugin" : True,
                       "grpc/*:shared" : True}

    src_folder = "core"


    def requirements(self):
        #override to new openSSL to fix version conflicts
        self.requires("openssl/3.1.0", override=True)
    
    def package_info(self):
        super().package_info()
        self.conan2_fix_cmake_names("foxtrotCore")

    def build(self):
        cmake = CMake(self)
        #need this e.g. to use grpc plugin if protoc is shared
        envvars = VirtualRunEnv(self).vars()

        cmake.configure()
        with envvars.apply():
            cmake.build()

        cmake.install()
        
