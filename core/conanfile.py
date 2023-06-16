import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps
from conan.tools.env import VirtualRunEnv, VirtualBuildEnv
from conan.tools.files import collect_libs


class FoxtrotCoreConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    exports_sources = "CMakeLists.txt", "src/*.cpp", "src/*.cc", \
                      "include/foxtrot/*.h", "include/foxtrot/*.hpp", \
                      "include/foxtrot/*.hh", \
                      "cmake/*.cmake", "cmake/*.cmake.in", \
                      "proto/*.proto", "share/*.in", "devprogs/*" ,\
                      "proto/CMakeLists.txt"

    cmake_package_name = "foxtrotCore"

    package_type = "shared-library"
    default_options = {"*/*:shared" : True,
                       "protobuf/*:with_zlib": True,
                       "grpc/*:csharp_ext" : False,
                       "grpc/*:node_plugin" : False,
                       "grpc/*:php_plugin" : False,
                       "grpc/*:ruby_plugin" : False,
                       "grpc/*:python_plugin" : True,
                       "rttr/*:shared" : True,
                       "rttr/*:with_rtti" : True
                       }

    src_folder = "core"

    def requirements(self):
        super().requirements()
        self.requires("rttr/0.9.6", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires("grpc/[^1.50.1]", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires("boost/[^1.82.0]", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True, override=True)



    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)
        deps.generate()

        tc = self._setup_cmake_tc()
        tc.generate()


    def build(self):
        cmake = CMake(self)
        #need this e.g. to use grpc plugin if protoc is shared
        envvars = VirtualRunEnv(self).vars()

        cmake.configure()
        with envvars.apply():
            cmake.build()

