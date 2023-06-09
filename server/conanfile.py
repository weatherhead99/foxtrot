from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain

class FoxtrotServerConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.0]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "*CMakeLists.txt", "src/*.cpp",\
    "include/exptserve/*.h", "include/foxtrot/server/*.h", \
    "include/foxtrot/server/*.hh", "include/foxtrot/devices/*.h",\
    "include/foxtrot/devices/*.hh", \
    "include/foxtrot/protocols/*.h", "include/exptserve/*.hh", \
    "cmake/*", "setups/*", "devprogs/*", "devices/*"
    requires = (
        "libsodium/[^1.0.18]",
        "rapidjson/[^1.1.0]",
#        "asio-grpc/[^2.4.0]"
                )

    default_options = {"*:shared" : True}
    src_folder="server"

    ft_package_requires = "core", "protocols"
    cmake_package_name = "foxtrotServer"
    package_type = "application"


    def deploy(self):
        self.copy("lib/foxtrot/dummy_setup.so", dst="setups", keep_path=False)

    def generate(self):
        #NOTE: need to alter target name of rapidJSON which differs from
        #upstream cmake build (I think)
        deps = CMakeDeps(self)
        deps.set_property("rapidjson", "cmake_target_name",
                          "RapidJSON::RapidJSON")
        deps.generate()

        tc = self._setup_cmake_tc()
        tc.generate()

