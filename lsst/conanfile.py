from conan import ConanFile

class FoxtrotLSST(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.1]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_lsst"
    description="LSST test stand specific stuff for foxtrot"
    exports_sources = ("CMakeLists.txt", "*/CMakeLists.txt", 
                       "testbench_setup_funcs.*", "tbprogs/*.h",
                       "tbprogs/*.cpp", "autofilld/*.cpp",
                       "autofilld/*.hh", "testbench_archon_heater.*",
                       "fsmd/*.cpp", "fsmd/include/*.hh", 
                       "motor_test_setup.cpp")
    src_folder= "lsst"
    ft_package_requires = ("core", "client" , "devices", "server")

    def requirements(self):
        super().requirements()
        self.requires("zlib/1.2.13", override=True)
        self.requires("grpc/1.54.3", override=True)

    def deploy(self):
        self.copy_deps("*exptserve")
        self.copy_deps("*dummy_setup.so")
        self.copy("*tbcli")
        self.copy("*autofilld")
        self.copy("*lsst_testbench.so")
