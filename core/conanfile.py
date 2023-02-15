import os
from conans import python_requires
from conan.tools.cmake import cmake_layout, CMake
from conans.tools import RunEnvironment, environment_append

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

    generators = "CMakeToolchain", "virtualrunenv", "CMakeDeps"

    def build(self):
        cmake = CMake(self)
        #remove to make new CMake helper error happy
        cmake.configure()
        env_build = RunEnvironment(self)
        with environment_append(env_build.vars):
            cmake.build()

    def package(self):
        self.output.warn(f"install path {self.package_folder}")
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotCore")

    def layout(self):
        cmake_layout(self)
        self.cpp.source.includedirs.append("include")
