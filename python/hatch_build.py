from hatchling.builders.hooks.plugin.interface import BuildHookInterface

from grpc_tools import command
from shutil import copyfile
import os

class ProtobufBuildHook(BuildHookInterface):
    PLUGIN_NAME="ft_protobuf"
    
    def clean(self):
        pass

    def initialize(self, version, build_data):
        self.app.display_info("in protobuf build hook!")

        print(f"directory: {self.directory}")
        print(f"root: {self.root}")
        print(f"config: {self.config}")

        if "proto_source_dir" not in self.config:
            raise ValueError("need to specify proto_source_dir")
        if "proto_output_dir" not in self.config:
            raise ValueError("need to specify proto_output_dir")

        sdir = self.config["proto_source_dir"]
        ddir = self.config["proto_output_dir"]
        proto_fls = [_ for _ in os.listdir(sdir) if ".proto" in _]
        self.app.display_info(f"protocol definition files: {proto_fls}")
        self.app.display_info(f"copying proto definitions...")

        for fl in proto_fls:
            copyfile(os.path.join(sdir, fl), os.path.join(ddir, fl))

        self.app.display_info(f"running protoc to generate python pb2 and grpc files")
        command.build_package_protos(ddir)

