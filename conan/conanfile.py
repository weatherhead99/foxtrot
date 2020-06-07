from conans import tools, ConanFile, CMake
import os

hardcode_version="0.0.1dev"

# def get_version():
#     git = tools.Git()
#     try:
#         tag = git.get_tag()
#         if git.get_tag() is not None:
#             return tag
#         else:
#             return "%s_%s" % (git.get_branch(), git.get_revision()[:8])
#     except:
#         return hardcode_version

def get_version():
    return hardcode_version

bcs = "@bincrafters/stable"
bbcs = "/1.69.0%s" % bcs

class FoxtrotCppMeta(type):
    def __new__(cls,name,bases,dct):
        if name != "FoxtrotCppPackage":
            newbases = list(bases)
            newbases.append(ConanFile)
        else:
            newbases = bases
        n = super().__new__(cls,name,tuple(newbases),dct)
        n.generators = "cmake", "virtualrunenv"
        n.settings = "os", "compiler", "build_type", "arch"
        return n

class FoxtrotBuildUtils(ConanFile):
    name = "FoxtrotBuildUtils"
    version = get_version()



class FoxtrotCppPackage(metaclass=FoxtrotCppMeta):
    default_user = "weatherhead99"
    default_channel = "testing"
    homepage = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    author = "Dan Weatherill (daniel.weatherill@physics.ox.ac.uk)"

    def set_version(self):
        git = tools.Git(folder=self.recipe_folder)
        tagged_version = git.get_tag()
        if tagged_version is None or tagged_version[0] != "v":
            self.version = "git%s" % git.get_revision()[:8]
        else:
            self.version = tagged_version[1:]

    def _setup_cmake(self):
        cmake = CMake(self)
        if self.develop:
            self.output.warn("in develop mode")
            cmake.definitions["FT_EXPORT_TO_PACKAGE_REGISTRY"] = True
        cmake.definitions["CONAN_PACKAGE_VERSION"] = self.version
        cmake.configure()
        return cmake
    
    def build(self):
        cmake = self._setup_cmake()
        env_build = tools.RunEnvironment(self)
        with tools.environment_append(env_build.vars):
            cmake.build()
        cmake.patch_config_paths()

    def package(self):
        cmake = self._setup_cmake()
        cmake.install()

    def _setup_libdirs_default(self, cppinfo):
        if not self.in_local_cache:
            if os.path.exists(".builddir.info"):
                self.output.info("found builddir info file")
                with open(".builddir.info","r") as f:
                    buildpath = f.read().strip()

                self.output.warn("buildpath: %s" % buildpath)
                cppinfo.libdirs = [os.path.join(buildpath,"lib"), buildpath]
                cppinfo.builddirs = [buildpath]
                cppinfo.includedirs.append(buildpath)

            self.output.warn("build folder: %s" % self.build_folder)
        else:
            cppinfo.libdirs = ["lib/foxtrot"]

    def package_info(self):
        self._setup_libdirs_default(self.cpp_info)
        self.cpp_info.libs = tools.collect_libs(self)
