from conans import tools, ConanFile, CMake

def get_version():
    git = tools.Git()
    try:
        tag = git.get_tag()
        if git.get_tag() is not None:
            return tag
        else:
            return "%s_%s" % (git.get_branch(), git.get_revision()[:8])
    except:
        return None

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
        with tools.environment_append(env_build.vars):
            cmake.build()
        cmake.install()

