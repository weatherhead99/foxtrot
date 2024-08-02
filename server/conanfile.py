from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain
from conan.tools.scm import Version
from conan.tools.build import check_min_cppstd

class FoxtrotServerConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.1]"
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
        "libsodium/[^1.0.19]",
        "rapidjson/[^1.1.0]"
                )
        
    default_options = {"boost:shared" : True,
                       "OpenSSL:shared" : True,
                       "libsodium:shared" : True,
                       "zlib:shared" : True}

    options = {"use_coro" : [True, False] }

    default_options = {"*/*:shared" : True,
                       "grpc/*:shared" : True,
                       "boost/*:shared" : True,
                       "use_coro": True}
    src_folder= "server"

    ft_package_requires = ( "core", "protocols")
    cmake_package_name = "foxtrotServer"
    package_type = "application"

    def config_options(self):
        if self.settings.compiler == "gcc":
            vers = Version(self.settings.compiler.version)
            print(vers)
            if vers < 10:
                self.output.info("gcc < 10, no use_coro option")
                self.options.rm_safe("use_coro")

    def validate_build(self):
        super().validate_build()
        if self.options.get_safe("use_coro"):
            check_min_cppstd(self, 20)


    def requirements(self):
        super().requirements()

        gcc_version = int(self.settings.compiler.version.value)
        if self.options.use_coro:
            if gcc_version <= 11:
                self.requires("asio-grpc/[<2.7]")
            else:
                self.requires("asio-grpc/[^2.9.2]")

        self.requires("boost/1.85.0", override=True)
        self.requires("zlib/1.2.13", override=True)
        self.requires("grpc/1.65.0", override=True,
                      transitive_headers=True,
                      transitive_libs=True)

        #fixing for upstream grpc package bug?
        self.requires("abseil/20240116.2", libs=True,
                      transitive_libs=True)




    def generate(self):
        #NOTE: need to alter target name of rapidJSON which differs from
        #upstream cmake build (I think)
        deps = CMakeDeps(self)
        deps.set_property("rapidjson", "cmake_target_name",
                          "RapidJSON::RapidJSON")
        deps.generate()

        tc = self._setup_cmake_tc()

        if "use_coro" in self.options:
            tc.cache_variables["USE_CORO_IMPLEMENTATIONS"] = bool(self.options.use_coro)
        else:
            tc.cache_variables["USE_CORO_IMPLEMENTATIONS"] = False

        tc.generate()
