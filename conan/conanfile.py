from conans import tools, ConanFile, CMake
from conan.tools.cmake import CMakeToolchain
import os


bcs = "@bincrafters/stable"
bbcs = "/1.69.0%s" % bcs

FT_HARDCODE_VERSION = "0.1.1"

class FoxtrotCppMeta(type):
    def __new__(cls,name,bases,dct):
        if name != "FoxtrotCppPackage":
            newbases = list(bases)
            newbases.append(ConanFile)
        else:
            newbases = bases
        n = super().__new__(cls,name,tuple(newbases),dct)
        return n

class FoxtrotBuildUtils(ConanFile):
    name = "FoxtrotBuildUtils"
    version = "0.2.1"
    
class FoxtrotCppPackage(metaclass=FoxtrotCppMeta):
    default_user = "weatherhead99"
    default_channel = "testing"
    homepage = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    author = "Dan Weatherill (daniel.weatherill@physics.ox.ac.uk)"
    generators = "CMakeToolchain", "cmake_find_package", "virtualrunenv"
    settings = "os", "compiler", "build_type", "arch"
    scm = {"type" : "git",
           "revision" : "auto"}
    

    def __init__(self, *args, **kwargs):
        if not hasattr(self, "src_folder"):
            raise RuntimeError("no src_folder attribute provided")
        self.scm["subfolder"] = self.src_folder
        super().__init__(*args, **kwargs)

    
    def set_version(self):
        git = tools.Git(folder=self.recipe_folder)
        tagged_version = git.get_tag()
        if tagged_version is None or tagged_version[0] != "v":
            self.version = FT_HARDCODE_VERSION
        else:
            self.version = tagged_version[1:]
    
    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_TOOLCHAIN_FILE"] = "conan_toolchain.cmake"
        cmake.configure()
        env_build = tools.RunEnvironment(self)
        with tools.environment_append(env_build.vars):
            cmake.build()
        cmake.install()
        cmake.patch_config_paths()
            
    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
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
 
