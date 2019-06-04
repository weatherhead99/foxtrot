import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotProtocolsConan(ftbase.FoxtrotCppPackage,
                            metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_protocols"
    description="protocol implementations for  foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h"
    requires = "libusb/1.0.22@bincrafters/stable", \
               "boost_asio%s" % ftbase.bbcs

    default_options = {"libusb:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_server/%s@%s/%s" % (self.version,self.user, self.channel))

