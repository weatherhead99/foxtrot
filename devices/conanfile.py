import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotDevicesConan(ftbase.FoxtrotCppPackage,
                          metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_devices"
    description="device drivers for foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "archon/*.cpp",\
                     "archon/*.h", "archon/CMakeLists.txt", "BME280/*.cpp",\
                     "BME280/*.h", "BME280/CMakeLists.txt" "BSC203/CMakeLists.txt", \
                     "BSC203/*.h", "BSC203/*.cpp", "cornerstone260/CMakeLists.txt", \
                     "cornerstone260/*.h", "cornerstone260/*.cpp", "DM3068/CMakeLists.txt",\
                     "DM3068/*.cpp", "DM3068/*.h", "newport_2936R/CMakeLists.txt", \
                     "newport2936R/*.h", "newport2936R/*.cpp" "OPS-Q250/CMakeLists.txt", \
                     "OPS-Q250/*.h", "OPS-Q250/*.cpp", "stellarnet/CMakeLists.txt", \
                     "stellarnet/*.h", "stellarnet/*.cpp", "TPG362/CMakeLists.txt", \
                     "TPG362/*.h", "TPG362/*.cpp", "webswitch_plus/CMakeLists.txt", \
                     "webswitch_plus/*.h", "webswitch_plus/*.cpp"

    requires = "OpenSSL/1.0.2r@conan/stable"
    default_options = {"OpenSSL:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" % (self.version, self.user, self.channel))
        self.requires("foxtrot_protocols/%s@%s/%s" % (self.version, self.user, self.channel))
