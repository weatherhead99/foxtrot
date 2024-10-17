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
    options = {"with_udev" : [True, False],
               "with_avahi" : [True, False]}
    default_options = {"*/*:shared" : True,
                       "protobuf/*:with_zlib": True,
                       "rttr/*:with_rtti" : True,
                       "absl/*:shared" : False,
                       "grpc/*:with_libsystemd" : False,
                       "boost/*:bzip2" : False,
                       "glib/*:with_mount" : False,
                       "glib/*:with_selinux" : False,
                       "with_udev" : True,
                       "with_avahi" : True
                       }

    src_folder = "core"

    CMake

    def requirements(self):
        super().requirements()
        protobuf_req: str = "protobuf/[^5.27.0]"
        self.tool_requires(protobuf_req)

        self.requires("grpc/[^1.65.0]", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires(protobuf_req, headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires("boost/1.86.0", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires("rttr/0.9.6", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        if self.options.with_udev:
            self.requires("libudev/system", headers=True, libs=True,
                          transitive_libs=True)

        if self.options.with_avahi:
            self.requires("avahi/0.8", headers=True, libs=True,
                          transitive_libs=True)
            #introduces a conflict... somehow
            self.requires("expat/2.6.3", override=True)

    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)

        if self.options.with_udev:
            deps.set_property("libudev", "cmake_target_aliases", ["PkgConfig::libudev"])

        if self.options.with_avahi:
            deps.set_property("avahi", "cmake_file_name", "avahi-client")
            deps.set_property("avahi::client", "cmake_target_name", "avahi-client::avahi_client")

            
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

    def package_info(self):
        super().package_info()
        self.cpp_info.resdirs = ["share/foxtrot"]
