import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")
bbcs = ftbase.bbcs
bcs = ftbase.bcs

class FoxtrotServerConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "CMakeLists.txt", "src/*.cpp", \
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/devices/*.h", "include/foxtrot/protocols/*.h", \
    "include/exptserve/*.hh", "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = "boost_program_options%s" % bbcs, \
        "libcurl/7.52.1%s" % bcs, \
        "libsodium/1.0.16%s" % bcs, \
        "cmake_findboost_modular%s" % bbcs, \
        "rapidjson/1.1.0%s" % bcs, \
    
    default_options = {"libcurl:shared" : True,
                       "OpenSSL:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version,self.user,self.channel))
        

    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)
