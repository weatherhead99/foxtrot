from conan import ConanFile
from conan.tools.cmake import CMakeDeps
from conan.tools.env import VirtualBuildEnv
from conan.tools.build import valid_min_cppstd
from conan.tools.gnu import PkgConfigDeps

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
    ft_package_requires = ("client", "devices", "server")

    default_options = {"*/*:shared" : True}

    deploy_setups = ("lsst_testbench.so",)

    def generate(self):
        super().generate()
        pcdeps = PkgConfigDeps(self)
        pcdeps.generate()

    def deploy(self):
        self.copy_deps("*exptserve")
        self.copy_deps("*dummy_setup.so")
        self.copy("*tbcli")
        self.copy("*autofilld")
        self.copy("*lsst_testbench.so")
