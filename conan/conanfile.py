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
        n = super().__new__(cls,name,bases,dct)
        n.generators = "cmake", "virtualrunenv"
        n.settings = "os", "compiler", "build_type", "arch"
        n.version = get_version()
        return n


class FoxtrotCppPackage(ConanFile):
    def build(self):
        cmake = CMake(self)
        cmake.configure()

        env_build = tools.RunEnvironment(self)
        cmake.definitions["CONAN_PACKAGE_VERSION"] = self.version
        with tools.environment_append(env_build.vars):
            cmake.build()
        cmake.install()
        cmake.patch_config_paths()

    def package_info(self):
        if not self.in_local_cache:
            if os.path.exists(".builddir.info"):
                self.output.info("found builddir info file")
                with open(".builddir.info","r") as f:
                    buildpath = f.read().strip()

                self.cpp_info.libdirs.append(os.path.join(buildpath,"lib"))
                self.cpp_info.includedirs.append(buildpath)
        
        self.output.warn("build folder: %s" % self.build_folder)

        self.cpp_info.libdirs = ["lib/foxtrot"]
        self.cpp_info.libs = tools.collect_libs(self)
