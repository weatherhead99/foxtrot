from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")

class FoxtrotLSST(ftbase.FoxtrotCppPackage,
                  metaclass = ftbase.FoxtrotCppMeta):
    name="foxtrot_lsst"
    description="LSST test stand specific stuff for foxtrot"
    exports_sources = ("CMakeLists.txt", "*/CMakeLists.txt", 
                       "testbench_setup_funcs.*", "tbprogs/*.h",
                       "tbprogs/*.cpp", "autofilld/*.cpp",
                       "autofilld/*.hh", "testbench_archon_heater.*",
                       "motor_test_setup.cpp")
    requires = "OpenSSL/1.0.2r@conan/stable"

    def requirements(self):
        self.requires("foxtrot_client/%s@%s/%s" % (self.version,self.user,self.channel))
        self.requires("foxtrot_protocols/%s@%s/%s" % (self.version,self.user,self.channel))
        self.requires("foxtrot_devices/%s@%s/%s" % (self.version,self.user,self.channel))

    def deploy(self):
        self.copy_deps("*exptserve")
        self.copy_deps("*dummy_setup.so")
        self.copy("*tbcli")
        self.copy("*autofilld")
        self.copy("*lsst_testbench.so")
