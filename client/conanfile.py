import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.2.1]@weatherhead99/stable")


class FoxtrotClientConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_client"
    description="foxtrot c++ client"
    exports_sources = "CMakeLists.txt", "cmake/*.in", \
                      "include/foxtrot/client/*.h", \
                      "src/*.cpp"

    src_folder = "client"

    def requirements(self):
        ftbase.ft_require(self, "core")

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotClient")
