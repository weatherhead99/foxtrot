import os
from conan import ConanFile
from conan.tools.env import VirtualRunEnv, VirtualBuildEnv
from conan.tools.cmake import CMakeDeps
from conan.tools.build import valid_min_cppstd
#ftbase = python_requires("FoxtrotBuildUtils/[^0.3]@weatherill/stable")

class FoxtrotProtocolsConan(ConanFile):
    python_requires="foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources= ("CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h",
                      "include/foxtrot/protocols/*.hh", "devprogs/CMakeLists.txt",
                      "devprogs/*.cpp")
    requires = ("libusb/[^1.0.26]",
                "libcurl/[^7.88.1]")

    options = {"use_asio_impls" : [True, False]}
    default_options = {"*/*:shared" : True,
                       "use_asio_impls" : True}


    package_type = "shared-library"
    ft_package_requires = "core",
    cmake_package_name = "foxtrotProtocols"

    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)
        deps.generate()

        tc = self._setup_cmake_tc()

        if self.options.use_asio_impls:
            tc.cache_variables["USE_ASIO"] = True

        tc.generate()

    def validate_build(self):
        if not valid_min_cppstd(self, 20):
            self.output.error(f"current cpp standard setting is: {self.settings.compiler.cppstd}")
            self.output.error("failed check requiring minimum of c++20")
            raise ConanInvalidConfiguration("foxtrot modules require at least c++20 standard to build")
