from hatchling.builders.hooks.plugin.interface import BuildHookInterface

from typing import Any
from grpc_tools import command
from shutil import copyfile, rmtree
import os

class ProtobufBuildHook(BuildHookInterface):
    PLUGIN_NAME="ft_protobuf"

    @property
    def _proto_output_dir(self) -> str:
        if "proto_output_dir" not in self.config:
            raise ValueError("need to specify proto_output_dir in config")
        return self.config["proto_output_dir"]


    def clean(self, versions: list[str]) -> None:
        if os.path.exists(self._proto_output_dir):
            self.app.display_info("cleaning proto output dir")
            rmtree(self._proto_output_dir)


    def initialize(self, version: str, build_data: dict[str, Any]) -> None:
        self.app.display_info("in protobuf build hook!")

        print(f"directory: {self.directory}")
        print(f"root: {self.root}")
        print(f"config: {self.config}")

        build_data["artifacts"].extend(["*.proto", "*.pyi", "*_pb2.py", "*_pb2_grpc.py"])
        
        if "proto_source_dir" not in self.config:
            raise ValueError("need to specify proto_source_dir")

        sdir = self.config["proto_source_dir"]
        ddir = self._proto_output_dir
        proto_fls = [_ for _ in os.listdir(sdir) if ".proto" in _]
        self.app.display_info(f"protocol definition files: {proto_fls}")
        self.app.display_info(f"copying proto definitions...")

        if not os.path.exists(ddir):
            self.app.display_info("target directory doesn't exist, making it")
            os.mkdir(ddir)
        
        for fl in proto_fls:
            copyfile(os.path.join(sdir, fl), os.path.join(ddir, fl))

        self.app.display_info(f"running protoc to generate python pb2 and grpc files")
        command.build_package_protos(ddir)


