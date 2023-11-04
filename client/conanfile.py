from conan import ConanFile


class FoxtrotClientConan(ConanFile):
    python_requires = "foxtrotbuildutils/[^0.4.1]"
    python_requires_extend = "foxtrotbuildutils.FoxtrotCppPackage"
    name="foxtrot_client"
    description="foxtrot c++ client"
    exports_sources = "CMakeLists.txt", "cmake/*.in", \
                      "include/foxtrot/client/*.h", \
                      "src/*.cpp"

    src_folder = "client"
    ft_package_requires = ("core")
    package_type = "shared-library"
    cmake_package_name = "foxtrotClient"

    default_options = {"*/*:shared" : True,
                       "grpc/*:shared" : True}

