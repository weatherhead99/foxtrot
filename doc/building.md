Building Foxtrot
================

Foxtrot itself is a fairly small and simple system, but it only achieves this through the extensive use of third party dependencies which implement a lot of the difficult parts. Thus, it is rather complex to build and get working. These dependencies can be provided either via system packages, or via the [conan](https://conan.io) package manager, which is currently the recommended method. It is in theory possible (though untested and not recommended) to use system packages for some dependencies and conan dependencies for others. Foxtrot requires a compiler fully supporting the c++17 standard.

Foxtrot is currently supported on GNU/Linux only, and only tested in the OpenSUSE Leap 15.1, ubuntu 16.04 and ubuntu 18.04 distributions. Porting to Windows is partially complete.

>**Important Note:** on ubuntu 18.04, the system grpc package has a bug which causes a segfault when running `exptserve`. In addition, the system grpc package does not provide the proper CMake config files. Thus, at the moment, on ubuntu 18.04, foxtrot is **only supported when building via conan (see below)**

Foxtrot Components
------------------

Foxtrot is split into several components, which can be built individually. These components are:

* **foxtrot_core**: contains the core library which consists of logging, error handling, some utility functions and compiled definitions for the foxtrot message types which are used by the other components

* **foxtrot_server**: contains the `exptserve` program, which is run on a machine which provides a foxtrot server. Also builds a "dummy" device and a setupfile containing it, allowing to play with a foxtrot server which doesn't have any real hardware devices attached for testing purposes

* **foxtrot_client**: contains the c++ foxtrot client library. Other language clients are implemented as their own packages

* **foxtrot_protocols**: contains drivers for common (and some not so common) communication protocols, which are used by device drivers

* **foxtrot_devices**: contains foxtrot drivers for hardware devices.

* **foxtrot_lsst**: builds the specific setup file and utility programs for using foxtrot on the OPMD LSST test stand

* **pyfoxtrot**: the foxtrot python client, written in pure python

Building with conan dependencies
--------------------------------

1. **install c++ build tools** You will need a c++ compiler (recommended system standard `gcc`), the `cmake` buildsystem, and a build tool (either GNU `make` or recommended the `ninja` build tool). To get these dependencies on Linux, use

	**ubuntu 18.04**

		sudo apt install ninja-build cmake cmake-qt-gui g++ 

	**openSUSE Leap 15.0**

		sudo zypper in gcc-c++ cmake-gui ninja 

2. **install conan package manager** First you need to have the `conan` package manager installed and working, in addition to a functional `python` (at least 3.4) environment. See [here](https://docs.conan.io/en/latest/installation.html) for detailed instructions on how to install conan. In brief, to install on ubuntu 18.04 use the following commands:

		$ sudo apt install python3-pip
		$ sudo pip install conan
				
	To check `conan` is installed properly, open a terminal and check (type in everything after the `$` symbol. Everything after the `>` symbol should be printed out on the console):


		$ conan --version
		> Conan version 1.21.1


	Conan should automatically pick up an installed system compiler settings the first time you run it. If you have installed the ninja build system, you need to change conan's settings to use it. Do this by editing the file `~/.conan/conan.conf`, and search for `cmake_generator`. Change this line to read

		cmake_generator=Ninja

3. **add conan remotes containing dependencies** Foxtrot's needed dependencies are scattered across multiple `conan` repositories (called "remotes" in `conan` parlance). To add the necessary remotes, issue these commands:

		$ conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan 
		$ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan 
		$ conan remote add weatherhead99 https://api.bintray.com/conan/weatherhead99/conan_packages

4. **build the foxtrot packages** To build all the packages (recommended), there is a python script which calls the conan API. From the top level foxtrot source directory, do

		$ cd conan
		$ python3 build_conan_packages.py
		
This will take a long time. After it is finished, you should have a set of foxtrot packages in your local `conan` cache.
