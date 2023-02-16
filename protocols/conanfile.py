import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.4]@weatherill/stable")

class FoxtrotProtocolsConan(ftbase.FoxtrotCppPackage):
#                            metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources="CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h"
    requires = ("libusb/[^1.0.26]",
                "libcurl/[^7.86.0]")

    default_options = {"libusb:shared" : True,
                       "libcurl:shared" : True}

    cmakeName = "foxtrotProtocols"

    def requirements(self):
        ftbase.ft_require(self, "core")

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotProtocols")
        self.cpp_info.set_property("cmake_find_mode", "none")

    def layout(self):
        super().layout()
        self.cpp.source.includedirs.append("include")
        self.cpp.build.builddirs=["."]
