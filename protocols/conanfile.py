import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.2.1@weatherhead99/testing")

class FoxtrotProtocolsConan(ftbase.FoxtrotCppPackage):
#                            metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h"
    requires = "libusb/1.0.26"

    default_options = {"libusb:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_server/%s@%s/%s" % (self.version,self.user, self.channel))

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotProtocols")
