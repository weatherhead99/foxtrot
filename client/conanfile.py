import os
from conans import python_requires

ftbase = python_requires("FoxtrotBuildUtils/[^0.4]@weatherill/stable")


class FoxtrotClientConan(ftbase.FoxtrotCppPackage,
                         metaclass=ftbase.FoxtrotCppMeta):
    name="foxtrot_client"
    description="foxtrot c++ client"
    exports_sources = "CMakeLists.txt", "cmake/*.in", \
                      "include/foxtrot/client/*.h", \
                      "src/*.cpp"

    src_folder = "client"
    cmakeName = "foxtrotClient"

    def requirements(self):
        ftbase.ft_require(self, "core")

    def package_info(self):
        super().package_info()
        self.fix_cmake_def_names("foxtrotClient")

    def layout(self):
        super().layout()
        self.cpp.source.includedirs.append("include")
        self.cpp.build.builddirs = ["."]
