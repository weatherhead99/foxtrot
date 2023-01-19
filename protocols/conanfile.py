import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.2.1]@weatherhead99/stable")

class FoxtrotProtocolsConan(ftbase.FoxtrotCppPackage):
#                            metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h"
    requires = "libusb/1.0.26"

    default_options = {"libusb:shared" : True}
    
    def requirements(self):
        ftbase.ft_require(self, "server")

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotProtocols")
