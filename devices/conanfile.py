import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotDevicesConan(ftbase.FoxtrotCppPackage,
                          metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_devices"
    description="device drivers for foxtrot"
    exports_sources= ("CMakeLists.txt", "cmake/*.in", "archon/*.cpp",
                      "archon/*.h", "BME280/*.cpp", "BME280/*.h",
                      "BSC203/*.h", "BSC203/*.cpp", "cornerstone_260/*.h",
                      "cornerstone_260/*.cpp", "DM3068/*.cpp", "DM3068/*.h",
                      "newport_2936R/*.h", "newport_2936R/*.cpp", "OPS-Q250/*.h",
                      "OPS-Q250/*.cpp", "stellarnet/*.h", "stellarnet/*.cpp",
                      "TPG362/*.h", "TPG362/*.cpp", "webswitch_plus/*.h",
                      "webswitch_plus/*.cpp", "*/CMakeLists.txt", "cmake/*.in")

    requires =  ("OpenSSL/1.0.2r@conan/stable",
                 "rapidxml/1.13@bincrafters/stable")
    default_options = {"OpenSSL:shared" : True,
                       "libcurl:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" % (self.version, self.user, self.channel))
        self.requires("foxtrot_protocols/%s@%s/%s" % (self.version, self.user, self.channel))

