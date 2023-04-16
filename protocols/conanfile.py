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

    package_type = "shared-library"
    ft_package_requires = "core",
    cmake_package_name = "foxtrotProtocols"

    overrides = "openssl/3.1.0",
