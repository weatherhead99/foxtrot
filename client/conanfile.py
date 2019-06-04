import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")


class FoxtrotClientConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_client"
    description="foxtrot c++ client"
    exports_sources = "CMakeLists.txt", "cmake/*.in", \
                      "include/foxtrot/client/*.h", \
                      "src/*.cpp"
    
    requires = "boost_program_options%s" % ftbase.bbcs, \
               "boost_log%s" % ftbase.bbcs, \
               "boost_filesystem%s" % ftbase.bbcs, \
               "protobuf/3.6.1%s" % ftbase.bcs, \
               "cmake_findboost_modular%s" % ftbase.bbcs
    default_options = {"boost_log:shared": True,
                       "boost_thread:shared": True,
                       "boost_program_options:shared": True,
                       "boost_filesystem:shared": True,
                       "boost_regex:shared": True,
                       "OpenSSL:shared": True,
                       "protobuf:with_zlib": True}   
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version, self.user, self.channel))

