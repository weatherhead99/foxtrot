from conans import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake
from conan.tools.scm import Git
from conan.tools.files import load, update_conandata
from conans.tools import environment_append, RunEnvironment, collect_libs
import os

class FoxtrotCppMeta(type):
    def __new__(cls,name,bases,dct):
        if name != "FoxtrotCppPackage":
            newbases = list(bases)
            newbases.append(ConanFile)
        else:
            newbases = bases
        n = super().__new__(cls,name,tuple(newbases),dct)
        return n

def ft_require(conanfile, substr: str) -> None:
    conanfile.requires(f"foxtrot_{substr}/[^{conanfile.version}]@{conanfile.user}/{conanfile.channel}")
    
class FoxtrotBuildUtils(ConanFile):
    name = "FoxtrotBuildUtils"
    version = "0.3"
    default_user = "weatherill"
    default_channel = "stable"
    
class FoxtrotCppPackage(metaclass=FoxtrotCppMeta):
    default_user = "weatherill"
    default_channel = "stable"
    homepage = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    url = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    author = "Dan Weatherill (daniel.weatherill@physics.ox.ac.uk)"
    generators = "CMakeToolchain", "cmake_find_package", "virtualrunenv"
    settings = "os", "compiler", "build_type", "arch"
    license = "UNLICENSED"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def export(self):
        git = Git(self, self.recipe_folder)
        if git.is_dirty():
            self.output.info("git repo is dirty, local create command will not capture SCM info")
            return
        scm_url, scm_commit = git.get_url_and_commit()
        update_conandata(self, {"sources" : {"commit" : scm_commit, "url" : scm_url}})

    def layout(self):
        self.folders.source = "."

    def set_version(self):
        git = Git(self, self.recipe_folder)
        gitversion = git.run("describe --tags")
        if gitversion[0] == "v":
            gitversion = gitversion[1:]
        self.version = gitversion
            
    def build(self):
        cmake = CMake(self)
        #remove to make new CMake helper error happy
        cmake.configure()
        env_build = RunEnvironment(self)
        with environment_append(env_build.vars):
            cmake.build()
        cmake.install()
            
    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.libdirs = ["lib/foxtrot"]
        self.cpp_info.names["cmake_find_package"] = "foxtrot"
        self.cpp_info.builddirs.append("lib")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def fix_cmake_def_names(self, cmakename: str):
        self.cpp_info.names["cmake_find_package"] = cmakename
        self.cpp_info.names["cmake_find_package_multi"] = cmakename
        self.cpp_info.builddirs.append("lib/cmake/%s" % cmakename)
 
