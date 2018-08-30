import os
from conans import ConanFile, CMake, tools

bcs = "@bincrafters/stable"
bbcs = "/1.66.0%s" % bcs


class FoxtrotConan(ConanFile):
    name="foxtrot"
    description="a simple device server"
    settings = "os", "compiler" , "build_type" , "arch"
    generators="cmake"
    default_options = "rttr:shared=True", "nanomsg:shared=True"

    requires= "grpc/1.14.1@inexorgame/stable", \
    "nanomsg/1.1.2%s" % bcs, \
    "rttr/0.9.6@weatherhead99/testing", \
    "boost_log%s" % bbcs, \
    "boost_date_time%s" % bbcs, \
    "boost_system%s" % bbcs, \
    "boost_asio%s" % bbcs, \
    "boost_filesystem%s" % bbcs, \
    "cmake_findboost_modular%s" % bbcs, \
    "boost_thread%s" % bbcs, \
    "libusb/1.0.22%s" % bcs, \
    "protobuf/3.5.2%s" % bcs

    def configure(self):
        #on linux, use shared protobuf & grpc builds
        if self.settings.os == "Linux":
            self.output.info("using shared protobuf build")
            self.options["protobuf"].shared = True


    
