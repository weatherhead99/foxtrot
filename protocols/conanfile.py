import os
from conan import ConanFile

#ftbase = python_requires("FoxtrotBuildUtils/[^0.3]@weatherill/stable")

class FoxtrotProtocolsConan(ConanFile):
    python_requires="foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h"
    requires = ("libusb/[^1.0.26]",
                "libcurl/[^7.88.1]")

    default_options = {"libusb/*:shared" : True,
                       "libcurl/*:shared" : True}
    
    def requirements(self):
        self.python_requires["foxtrotbuildutils"].module.ft_require(self, "core")

    def package_info(self):
        super().package_info()
        self.conan2_fix_cmake_names("foxtrotProtocols")
