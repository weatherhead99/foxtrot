from setuptools import setup, find_packages

setup(
      name='pyfoxtrot',
      version='0.0',
      description='python bindings for foxtrot client and telemetry client',
      author='Dan Weatherill',
      author_email='daniel.weatherill@physics.ox.ac.uk',
      setup_requires = [
              "grpcio-tools"],
      packages = find_packages(),
      include_package_data = True,
      install_requires = ["mypy", "grpcio-tools"]
      )
