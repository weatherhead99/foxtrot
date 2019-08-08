import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotCoreConan(ftbase.FoxtrotCppPackage,
                       metaclass=ftbase.FoxtrotCppMeta):
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    exports_sources = "CMakeLists.txt", "src/*.cpp", "src/*.cc", \
                      "include/foxtrot/*.h", "include/foxtrot/*.hpp", \
                      "include/foxtrot/*.hh", \
                      "cmake/*.cmake", "cmake/*.cmake.in", \
                      "proto/*.proto", "share/*.in", "devprogs/*"
    requires = "boost_log%s" % ftbase.bbcs, \
        "boost_thread%s" % ftbase.bbcs, \
        "boost_program_options%s" % ftbase.bbcs, \
        "boost_filesystem%s" % ftbase.bbcs, \
        "grpc/1.20.0@inexorgame/stable",\
        "protobuf/3.6.1%s" % ftbase.bcs, \
        "cmake_findboost_modular%s" % ftbase.bbcs, \
        "OpenSSL/1.0.2s@conan/stable", \
        "rttr/0.9.6@weatherhead99/testing"

    default_options = {"boost_log:shared": True,
                       "boost_thread:shared": True,
                       "boost_program_options:shared": True,
                       "boost_filesystem:shared": True,
                       "boost_regex:shared": True,
                       "OpenSSL:shared": True,
                       "protobuf:with_zlib": True,
                       "protobuf:shared" : True,
                       "rttr:shared" : True}
