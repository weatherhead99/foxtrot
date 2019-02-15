import os
from conans import ConanFile, CMake, tools

bcs = "@bincrafters/stable"
bbcs = "/1.66.0%s" % bcs


class FoxtrotConan(ConanFile):
    name="foxtrot"
    description="a simple device server"
    settings = "os", "compiler" , "build_type" , "arch"
    generators="cmake", "virtualrunenv"
    default_options = "rttr:shared=True", "nanomsg:shared=True", \
                      "mosquitto:shared=True"

    requires= "grpc/1.17.2@inexorgame/stable", \
    "nanomsg/1.1.2%s" % bcs, \
    "rttr/0.9.6@weatherhead99/testing", \
    "boost_log%s" % bbcs, \
    "boost_date_time%s" % bbcs, \
    "boost_system%s" % bbcs, \
    "boost_test%s" % bbcs, \
    "boost_program_options%s" % bbcs, \
    "boost_asio%s" % bbcs, \
    "boost_filesystem%s" % bbcs, \
    "boost_thread%s" % bbcs, \
    "libusb/1.0.22%s" % bcs, \
    "protobuf/3.6.1%s" % bcs, \
    "mosquitto/1.4.15%s" % bcs, \
    "libcurl/7.61.1%s" % bcs, \
    "cmake_findboost_modular%s" % bbcs, \
    "OpenSSL/1.0.2q@conan/stable"

        
    def configure(self):
        pass
        #on linux, use shared protobuf & grpc builds
        #if self.settings.os == "Linux":
            #self.output.info("using shared protobuf build")
            #self.options["protobuf"].shared = True


    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", dst="bin", src="bin")
            if self.settings.build_type == "Debug":
                self.copy("*.pdb", dst="bin", src="bin")
            

    def build(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_DASHBOARD"] = "OFF"
        cmake.configure()

        
        env_build = tools.RunEnvironment(self)
        with tools.environment_append(env_build.vars):
            cmake.build()
        
