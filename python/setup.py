from setuptools import setup

setup(
      name='foxtrot',
      version='0.0',
      description='python bindings for foxtrot client and telemetry client',
      author='Dan Weatherill',
      author_email='daniel.weatherill@physics.ox.ac.uk',
      install_requires = [  
              'grpcio-tools',
              'nanomsg >= 1.0',
              'compiler',
              'visitor'
              ]
      
      )
