import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")
bbcs = ftbase.bbcs
bcs = ftbase.bcs

class FoxtrotServerConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    exports_sources = "cmake/*"
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "CMakeLists.txt", "src/*.cpp", \
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/devices/*.h", "include/foxtrot/protocols/*.h", \
    "include/exptserve/*.hh", "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = "boost_program_options%s" % bbcs, \
        "boost_filesystem%s" % bbcs, \
        "libcurl/7.61.1%s" % bcs, \
        "libsodium/1.0.16%s" % bcs, \
        "cmake_findboost_modular%s" % bbcs, \
        "rttr/0.9.6@weatherhead99/testing"
    
    default_options = {"rttr:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version,self.user,self.channel))
        
    def package_info(self):
        self.cpp_info.libs=["libfoxtrot_server.so"]
        self.cpp_info.libdirs=["lib/foxtrot"]

    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)
