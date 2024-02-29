import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class FoxtrotConan(ConanFile):
    name="foxtrot"
    description="a simple device server"
    settings = "os", "compiler" , "build_type" , "arch"
    generators = "CMakeDeps", "CMakeToolchain", "VirtualRunEnv", "VirtualBuildEnv"
        
    def requirements(self):
        for pkg in ["core", "protocols", "devices", "server", "client"]:
            self.requires(f"foxtrot_{pkg}/[~{self.version},include_prerelease]")

    def layout(self):
        cmake_layout(self, src_folder="conansrc", build_folder="conanbuild")
    
