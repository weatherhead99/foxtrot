import os
from conan import ConanFile
from conan.tools.cmake import CMakeDeps

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

    requires =  ("rapidxml/1.13",
                 "libusb/[^1.0.26]")
    package_type = "shared-library"

    ft_package_requires = "protocols","core"
    cmake_package_name = "foxtrotDevices"

    def generate(self):
        super().generate()
        rttr = self.dependencies["rttr"]
        self.output.info(f"is rttr build_context? {rttr.is_build_context}")
        self.output.info(f"rttr pref: {rttr.pref}")
        self.output.info(f"rttr cpp_info: {rttr.cpp_info}")

        reqclause = ( k for k,v in self.dependencies.items() if v is self.dependencies["rttr"])

        req2 = next(reqclause)

        self.output.info(f"rttr transitive headers: {req2.transitive_headers}")
        self.output.info(f"rttr headers: {req2.headers}")
