import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps
from conan.tools.env import VirtualRunEnv, VirtualBuildEnv
from conan.tools.files import collect_libs
from conan.tools.scm import Version

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
                       "with_avahi" : True,
                       "grpc/*:secure" : True
                       }

    src_folder = "core"

    def build_requirements(self):
        self.tool_requires("grpc/<host_version>")
        #note original RTTR fails to build with cmake > 4
        #(will not be patched upstream any time soon)

    def validate_build(self):
        cvers = Version(self.settings.compiler.version)
        if cvers >= 15:
            raise ConanInvalidConfiguration("cannot build with gcc > 15 yet")

    def config_options(self):
        #no libudev or avahi on windows, and grpc must not be shared
        if self.settings.os == "Windows":
            self.options.rm_safe("with_udev")
            self.options.rm_safe("with_avahi")

    def requirements(self):
        super().requirements()

        self.requires("grpc/1.72.0", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)
        #Note: using protobuf or grpc shared appears to need this
        self.requires("protobuf/5.27.0", force=True, transitive_libs=True)

        self.requires("boost/1.86.0", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        self.requires("rttr/0.9.6", headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        if self.options.get_safe("with_udev"):
            self.requires("libudev/system", headers=True, libs=True,
                          transitive_libs=True)

        if self.options.get_safe("with_avahi"):
            self.requires("avahi/0.8", headers=True, libs=True,
                          transitive_libs=True)

    def generate(self):
        buildenv = VirtualBuildEnv(self)
        runenv = VirtualRunEnv(self)
        buildenv.generate()
        runenv.generate()

        deps = CMakeDeps(self)
        tc = self._setup_cmake_tc()

        if self.options.get_safe("with_udev"):
            deps.set_property("libudev", "cmake_target_aliases", ["PkgConfig::libudev"])
            tc.cache_variables["BUILD_UDEV_SUPPORT"] = True
        else:
            tc.cache_variables["BUILD_UDEV_SUPPORT"] = False

        if self.options.get_safe("with_avahi"):
            deps.set_property("avahi", "cmake_file_name", "avahi-client")
            deps.set_property("avahi::client", "cmake_target_name", "avahi-client::avahi_client")
            tc.cache_variables["BUILD_AVAHI_SUPPORT"] = True
        else:
            tc.cache_variables["BUILD_AVAHI_SUPPORT"] = False


        deps.generate()

        tc.generate()


    def build(self):
        cmake = CMake(self)
        #need this e.g. to use grpc plugin if protoc is shared
        envvars_run = VirtualRunEnv(self).vars()

        cmake.configure()
        with envvars_run.apply() :
            cmake.build()

    def package_info(self):
        super().package_info()
        self.cpp_info.resdirs = ["share/foxtrot"]
