import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotCoreConan(ftbase.FoxtrotCppPackage,
                       metaclass=ftbase.FoxtrotCppMeta):
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    exports_sources = "CMakeLists.txt", "src/*.cpp", \
                      "include/foxtrot/*.h", "include/foxtrot/*.hpp", \
                      "cmake/*.cmake", "cmake/*.cmake.in", \
                      "proto/*.proto", "share/*.in"
    requires = "boost_log%s" % ftbase.bbcs, \
        "boost_program_options%s" % ftbase.bbcs, \
        "boost_filesystem%s" % ftbase.bbcs, \
        "grpc/1.17.2@inexorgame/stable",\
        "protobuf/3.6.1%s" % ftbase.bcs, \
        "cmake_findboost_modular%s" % ftbase.bbcs, \
        "OpenSSL/1.0.2q@conan/stable"
