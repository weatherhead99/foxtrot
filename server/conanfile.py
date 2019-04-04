import os
from conans import ConanFile, CMake, tools

bcs = "@bincrafters/stable"
bbcs = "/1.69.0%s" % bcs

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

class FoxtrotServerConan(ConanFile):
    name="foxtrot_server"
    description="foxtrot server components"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "*"
    version = get_version()
    requires = "boost_program_options%s" % bbcs, \
        "boost_filesystem%s" % bbcs, \
        "libcurl/7.61.1%s" % bcs
    
    def requirements(self):
        self.requires("foxtrot_core/%s@%s/%s" % 
                      (self.version,self.user,self.channel))
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()
