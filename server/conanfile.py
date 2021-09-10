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
    "include/foxtrot/server/*.hh", "include/foxtrot/devices/*.h",\
    "include/foxtrot/protocols/*.h", "include/exptserve/*.hh", \
    "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = ("boost/1.73.0", 
                "libsodium/1.0.18",  
                "rapidjson/1.1.0", 
                "libcurl/7.52.1@bincrafters/stable",  #old version for grpc
                "openssl/1.0.2t", #this specific version needed for grpcx
                "zlib/1.2.11")
    
    default_options = {"libcurl:shared" : True,
                       "OpenSSL:shared" : True,
                       "libsodium:shared" : True}
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version,self.user,self.channel))
        

    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)
