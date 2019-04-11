import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.1@weatherhead99/testing")
bbcs = ftbase.bbcs
bcs = ftbase.bcs

class FoxtrotServerConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_server"
    description="foxtrot server components"
    exports_sources = "CMakeLists.txt", "src/*.cpp",
    "include/exptserve/*.h", "include/foxtrot/server/*.h",
    "cmake/*.in"
    requires = "boost_program_options%s" % bbcs, \
        "boost_filesystem%s" % bbcs, \
        "libcurl/7.61.1%s" % bcs

    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version,self.user,self.channel))
