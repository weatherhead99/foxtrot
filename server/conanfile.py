import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.2.1]@weatherhead99/stable")

class FoxtrotServerConan(ftbase.FoxtrotCppPackage):
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "*CMakeLists.txt", "src/*.cpp",\
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/server/*.hh", "include/foxtrot/devices/*.h",\
    "include/foxtrot/protocols/*.h", "include/exptserve/*.hh", \
    "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = (
                "libcurl/7.86.0",
                "libsodium/1.0.18",
                "rapidjson/1.1.0",
                )
        
    default_options = {"boost:shared" : True,
                       "libcurl:shared" : True,
                       "OpenSSL:shared" : True,
                       "libsodium:shared" : True,
                       "zlib:shared" : True}

    src_folder="server"
    
    def requirements(self):
        ftbase.ft_require(self, "core")
        
    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)


    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotServer")
