from conan import ConanFile

class FoxtrotServerConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "*CMakeLists.txt", "src/*.cpp",\
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/server/*.hh", "include/foxtrot/devices/*.h",\
    "include/foxtrot/protocols/*.h", "include/exptserve/*.hh", \
    "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = (
                "libsodium/[^1.0.18]",
                "rapidjson/[^1.1.0]",
                )

    default_options = {"boost/*:shared" : True,
                       "libsodium/*:shared" : True,
                       "zlib/*:shared" : True}

    src_folder="server"

    ft_package_requires = "core", "protocols"
    cmake_package_name = "foxtrotServer"

    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)
