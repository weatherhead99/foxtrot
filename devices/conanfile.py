
import os
from conan import ConanFile
from conan.tools.cmake import CMakeDeps
from conan.tools.build import valid_min_cppstd
from conan.tools.gnu import PkgConfigDeps

class FoxtrotDevicesConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_devices"
    src_folder="devices"
    description="device drivers for foxtrot"

    exports_sources = ("*CMakeLists.txt", "cmake/*.in",
                       "*/*.cpp", "*/*.h", "*/*.hh", "cmake/Find*.cmake")

    requires =  ("libusb/[^1.0.26]")

    package_type = "shared-library"
    default_options = {"*/*:shared" : True}
    ft_package_requires = ("protocols")
    cmake_package_name = "foxtrotDevices"

    def requirements(self):
        super().requirements()
        self.requires("rapidxml/1.13",
                      headers=True, libs=True,
                      transitive_headers=True,
                      transitive_libs=True)

        #need libaravis
        self.requires("aravis/0.8.33")
        #override, conflict aravis with avahi
        self.requires("glib/2.78.3", override=True)
        

    def generate(self):
        #do all the cmake toolchain stuff etc
        super().generate()

        #manually set up pkgconfig file for aravis,
        #that's how we find it generally
        pcdeps = PkgConfigDeps(self)
        pcdeps.generate()

