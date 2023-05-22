import os
from conan import ConanFile

#ftbase = python_requires("FoxtrotBuildUtils/[^0.3]@weatherill/stable")

class FoxtrotProtocolsConan(ConanFile):
    python_requires="foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_protocols"
    src_folder="protocols"
    description="protocol implementations for  foxtrot"
    exports_sources= ("CMakeLists.txt", "cmake/*.in", "src/*.cpp", "include/foxtrot/protocols/*.h",
                      "include/foxtrot/protocols/*.hh")
    requires = ("libusb/[^1.0.26]",
                "libcurl/[^7.88.1]")

    default_options = {"*/*:shared" : True}

    package_type = "shared-library"
    ft_package_requires = "core",
    cmake_package_name = "foxtrotProtocols"

    overrides = "openssl/3.1.0",

    def generate(self):
        super().generate()
        rttr = self.dependencies["rttr"]
        self.output.info(f"is rttr build_context? {rttr.is_build_context}")
        self.output.info(f"rttr pref: {rttr.pref}")
        self.output.info(f"rttr cpp_info: {rttr.cpp_info}")

        reqclause = ( k for k,v in self.dependencies.items() if v is self.dependencies["rttr"])

        req2 = next(reqclause)

        self.output.info(f"rttr transitive headers: {req2.transitive_headers}")
        self.output.info(f"rttr headers: {req2.headers}")
