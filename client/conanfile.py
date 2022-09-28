import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/0.2.1@weatherhead99/testing")


class FoxtrotClientConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_client"
    description="foxtrot c++ client"
    exports_sources = "CMakeLists.txt", "cmake/*.in", \
                      "include/foxtrot/client/*.h", \
                      "src/*.cpp"

    src_folder = "client"

    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" %
                      (self.version, self.user, self.channel))

