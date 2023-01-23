import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.3]@weatherill/stable")

class FoxtrotDevicesConan(ftbase.FoxtrotCppPackage,
                          metaclass = ftbase.FoxtrotCppMeta):
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

    requires =  ("rapidxml/1.13")
    
    
    def requirements(self):
        ftbase.ft_require(self, "core")
        ftbase.ft_require(self, "protocols")


    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotDevices")
