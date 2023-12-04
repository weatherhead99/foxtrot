import os
from conan import ConanFile
from conan.tools.cmake import CMakeDeps
from conan.tools.build import valid_min_cppstd

class FoxtrotDevicesConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_devices"
    src_folder="devices"
    description="device drivers for foxtrot"
    exports_sources= ("CMakeLists.txt", "cmake/*.in", "archon/*.cpp",
                      "archon/*.h", "BME280/*.cpp", "BME280/*.h",
                      "Thorlabs_APT/*.h", "Thorlabs_APT/*.hh", "Thorlabs_APT/*.cpp", "cornerstone_260/*.h",
                      "cornerstone_260/*.cpp", "DM3068/*.cpp", "DM3068/*.h",
                      "newport_2936R/*.h", "newport_2936R/*.cpp", "OPS-Q250/*.h",
                      "OPS-Q250/*.cpp", "stellarnet/*.h", "stellarnet/*.cpp",
                      "TPG362/*.h", "TPG362/*.cpp", "webswitch_plus/*.h",
                      "DLP-IOR4/*.cpp", "DLP-IOR4/*.hh",
                      "webswitch_plus/*.cpp", "*/CMakeLists.txt", "cmake/Find*.cmake",
                      "idscamera/*.cpp", "idscamera/*.h")

    requires =  ("libusb/[^1.0.26]")


    def requirements(self):
        super().requirements()
        self.requires("rapidxml/1.13",
                      headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

    def validate_build(self):
        if not valid_min_cppstd(self, 20):
            self.output.error(f"current cpp standard setting is: {self.settings.compiler.cppstd}")
            self.output.error("failed check requiring minimum of c++20")
            raise ConanInvalidConfiguration("foxtrot modules require at least c++20 standard to build")


    package_type = "shared-library"
    default_options = {"*/*:shared" : True}
    ft_package_requires = ("protocols")
    cmake_package_name = "foxtrotDevices"

