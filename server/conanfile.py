import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.2.1@weatherhead99/testing")

class FoxtrotServerConan(ftbase.FoxtrotCppPackage):
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "*CMakeLists.txt", "src/*.cpp",\
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/server/*.hh", "include/foxtrot/devices/*.h",\
    "include/foxtrot/protocols/*.h", "include/exptserve/*.hh", \
    "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = ("boost/1.78.0",
                "openssl/1.1.1m",
                "libcurl/7.77.0",
                "libsodium/1.0.18",
                "rapidjson/1.1.0",
                "zlib/1.2.11")
        
    default_options = {"boost:shared" : True,
                       "libcurl:shared" : True,
                       "OpenSSL:shared" : True,
                       "libsodium:shared" : True,
                       "zlib:shared" : True}

    src_folder="server"
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version,self.user,self.channel))
        

    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)
