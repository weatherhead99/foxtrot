import os
from conans import ConanFile, CMake, tools

bcs = "@bincrafters/stable"
bbcs = "/1.69.0%s" % bcs

def get_version():
    git = tools.Git()
    try:
        return "%s_%s" % (git.get_branch(), git.get_revision())
    except:
        return None

class FoxtrotCoreConan(ConanFile):
    name = "foxtrot_core"
    description = "core libraries for foxtrot"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "virtualrunenv"
    exports_sources = "*"
    version = get_version()
    requires = "boost_log%s" % bbcs, \
        "grpc/1.17.2@inexorgame/stable",\
        "protobuf/3.6.1%s" % bcs, \
        "cmake_findboost_modular%s" % bbcs
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        
        env_build = tools.RunEnvironment(self)
        with tools.environment_append(env_build.vars):
            cmake.build()
        
        cmake.install()
