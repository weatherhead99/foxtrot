from conans import python_requires, tools

ftbase = python_requires("FoxtrotBuildUtils/[^0.4]@weatherill/stable")

class FoxtrotLSST(ftbase.FoxtrotCppPackage):
    name="foxtrot_lsst"
    description="LSST test stand specific stuff for foxtrot"
    exports_sources = ("CMakeLists.txt", "*/CMakeLists.txt", 
                       "testbench_setup_funcs.*", "tbprogs/*.h",
                       "tbprogs/*.cpp", "autofilld/*.cpp",
                       "autofilld/*.hh", "testbench_archon_heater.*",
                       "fsmd/*.cpp", "fsmd/include/*.hh", 
                       "motor_test_setup.cpp")
    src_folder="lsst"
    
    def requirements(self):
        ftbase.ft_require(self, "client")
        ftbase.ft_require(self, "protocols")
        ftbase.ft_require(self, "devices")

        #this is only needed as a dep to run the exptserve binary conveniently
        ftbase.ft_require(self, "server")

    def deploy(self):
        self.copy_deps("*exptserve")
        self.copy_deps("*dummy_setup.so")
        self.copy("*tbcli")
        self.copy("*autofilld")
        self.copy("*lsst_testbench.so")

    def package_info(self):
        super().package_info()
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = tools.collect_libs(self)
