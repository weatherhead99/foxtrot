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
    requires = "boost/1.73.0", \
        "grpc/1.25.0@inexorgame/stable",\
        "rttr/0.9.6@weatherhead99/testing", \

    default_options = {"boost:shared" : True,
                       "OpenSSL:shared": True,
                       "protobuf:with_zlib": True,
                       "protobuf:shared" : True,
                       "rttr:shared" : True}
