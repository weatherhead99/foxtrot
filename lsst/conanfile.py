from conan import ConanFile
from conan.tools.cmake import CMakeDeps
from conan.tools.env import VirtualBuildEnv


class FoxtrotLSST(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.1]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_lsst"
    description="LSST test stand specific stuff for foxtrot"
    exports_sources = ("CMakeLists.txt", "*/CMakeLists.txt", 
                       "testbench_setup_funcs.*", "tbprogs/*.h",
                       "chiller_setup_funcs.*",
                       "tbprogs/*.cpp", "autofilld/*.cpp",
                       "autofilld/*.hh", "testbench_archon_heater.*",
                       "fsmd/*.cpp", "fsmd/include/*.hh", 
                       "motor_test_setup.cpp")
    src_folder = "lsst"
    ft_package_requires = ("core", "client" , "devices", "server")

    options = {"magis_chiller" : [True, False]}
    default_options = {"*/*:shared" : True,
                       "magis_chiller" : False}

    deploy_setups = ("lsst_testbench.so")

    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)
        deps.generate()

        tc = self._setup_cmake_tc()
        tc.variables["MAGIS_CHILLER"] = self.options.magis_chiller
        tc.generate()

    def requirements(self):
        super().requirements()
        self.requires("zlib/1.2.13", override=True)
        self.requires("grpc/1.54.3", override=True)

        if self.options.magis_chiller:
            self.requires("foxtrot_magis/[^0.0.1-a1,include_prerelease=True]")

    def deploy(self):
        self.copy_deps("*exptserve")
        self.copy_deps("*dummy_setup.so")
        self.copy("*tbcli")
        self.copy("*autofilld")
        self.copy("*lsst_testbench.so")
