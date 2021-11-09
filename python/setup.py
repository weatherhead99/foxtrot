from setuptools import setup, find_packages, Command
from setuptools.command.build_py import build_py
import os

PROTO_DIR = os.path.abspath(os.path.join("pyfoxtrot", "protos"))

class simplecommand(Command):
    def initialize_options(self):
        pass
    def finalize_options(self):
        pass

class BuildPackageProtos(simplecommand):
    description = "compile grc and protobuf python modules"
    user_options = []

    def run(self):
        print("building protos...")
        proto_files = [_ for _ in os.listdir(PROTO_DIR) if ".proto" in _]
        print("proto files found: %s" % proto_files)

        if len(proto_files) == 0:
            raise RuntimeError("failed to find any .proto files. Perhaps you need to build the c++ component first!")

        from grpc.tools import command
        command.build_package_protos(PROTO_DIR)

class BuildPyCommand(build_py):
    def run(self):
        self.run_command("build_proto_modules")
        print("py modules: %s" % self.py_modules)
        print("packages: %s" % self.packages)
        self.packages = find_packages()
        print("packages: %s" % self.packages)
        build_py.run(self)
        

setup(
      name='pyfoxtrot',
      version='0.1',
      description='python bindings for foxtrot client and telemetry client',
      author='Dan Weatherill',
      author_email='daniel.weatherill@physics.ox.ac.uk',
      setup_requires = [
              "grpcio-tools"],
      packages = find_packages(),
      include_package_data = True,
      cmdclass = {"build_proto_modules" : BuildPackageProtos,
                "build_py" : BuildPyCommand},
      install_requires = ["mypy", "grpcio-tools", "grpcio"]
      )
