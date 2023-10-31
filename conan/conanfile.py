from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps
from conan.tools.scm import Git, Version
from conan.tools.files import load, update_conandata
from conan.tools.env import Environment, VirtualRunEnv, VirtualBuildEnv
from conan.tools.files import  collect_libs
from conan.tools.cmake import cmake_layout
from conan.tools.build import valid_min_cppstd
import os

FOXTROT_GLOBAL_OVERRIDES = []

class FoxtrotCppMeta(type):
    BASE_PKG_NAME = "FoxtrotCppPackage"

    def __new__(cls,name,bases,dct):
        if name != cls.BASE_PKG_NAME:
            newbases = list(bases)
            newbases.append(ConanFile)
        else:
            newbases = bases
        n = super().__new__(cls,name,tuple(newbases),dct)

        if name != cls.BASE_PKG_NAME:
            #merge in the options
            print("merging in the options and default options")
            basepkg = [_.__name__ for _ in bases].index(cls.BASE_PKG_NAME)

            n.options = getattr(n, "options", {}) | bases[basepkg].options
            n.default_options = getattr(n, "default_options", {}) | bases[basepkg].default_options

            print(f" options: {n.options}")
            print(f"default options: {n.default_options}")
        return n

def ft_require(conanfile, substr: str) -> None:
    if getattr(conanfile, "stable_release", False):
        conanfile.requires(f"foxtrot_{substr}/[~{conanfile.version}]")
    else:
        reqstr = ft_version_get_req_str(conanfile.version)
        conanfile.requires(f"foxtrot_{substr}/{reqstr}")

class FoxtrotBuildUtils(ConanFile):
    name = "foxtrotbuildutils"
    version = "0.4.1"
    default_user = "weatherill"
    default_channel = "stable"
    package_type = "python-require"
    revision_mode= "scm_folder"


def add_grpc_options(configs):
    for opt_name in ["csharp_ext", "node_plugin", "php_plugin", "ruby_plugin", "python_plugin"]:
        fulloptname = f"grpc/*:{opt_name}"
        configs.append({"options" : [(fulloptname, True), (fulloptname, False)]})

    return configs


    
class FoxtrotCppPackage:
    default_user = "weatherill"
    default_channel = "stable"
    homepage = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    url = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    author = "Dan Weatherill (daniel.weatherill@physics.ox.ac.uk)"
    settings = "os", "compiler", "build_type", "arch"
    license = "UNLICENSED"
    tool_requires = "cmake/[>=3.20.0]"
    revision_mode = "scm_folder"

#    def compatibility(self):
#        configs = []
#        return add_grpc_options(configs)

    def __init_subclass__(cls):
        pass

    def export(self):
        git = Git(self, self.recipe_folder)
        self.output.info(f"version: {self.version}")
        update_conandata(self, {"frozen_version" : self.version})
        if git.is_dirty():
            self.output.info("git repo is dirty, local create command will not capture SCM info")
            return
        scm_url, scm_commit = git.get_url_and_commit()
        update_conandata(self, {"sources" : {"commit" : scm_commit, "url" : scm_url},
                                })

    def validate_build(self):
        if not valid_min_cppstd(self, 17):
            self.output.error(f"current cpp standard setting is: {self.settings.compiler.cppstd}")
            self.output.error("failed check requiring minimum of c++17")
            raise ConanInvalidConfiguration("foxtrot modules require at least c++17 standard to build")

    def set_version(self):
        if hasattr(self, "conandata") and "frozen_version" in self.conandata:
            self.version = self.conandata["frozen_version"]
        else:
            git = Git(self, self.recipe_folder)
            if not self.version:
                self.version = semver_from_git_describe(git)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.libdirs = ["lib/foxtrot"]
        self.cpp_info.builddirs.append("lib")

        if hasattr(self, "cmake_package_name"):
            self.conan2_fix_cmake_names(self.cmake_package_name)

    def requirements(self):
        if hasattr(self, "ft_package_requires"):
            if isinstance(self.ft_package_requires, str):
                ft_require(self, self.ft_package_requires)
                return

            for pack in self.ft_package_requires:
                self.output.info(f"adding other foxtrot package {pack} to dependencies")
                ft_require(self, pack)

        # if hasattr(self, "overrides"):
        #     for pack in self.overrides:
        #         self.output.info(f"got override {pack} from local overrides")
        #         self.requires(pack, override=True)
        #         FOXTROT_GLOBAL_OVERRIDES.append(pack)


    def _setup_cmake_tc(self):
        tc = CMakeToolchain(self)
        tc.variables["VERSION_FROM_CONAN"] = self.version
        return tc


    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)
        deps.generate()

        tc = self._setup_cmake_tc()
        tc.generate()

    def fix_cmake_def_names(self, cmakename: str):
        self.cpp_info.names["cmake_find_package"] = cmakename
        self.cpp_info.names["cmake_find_package_multi"] = cmakename
        self.cpp_info.builddirs.append("lib/cmake/%s" % cmakename)

    def conan2_fix_cmake_names(self, cmakename: str):
        self.cpp_info.builddirs.append(f"lib/cmake/{cmakename}")
        #self.cpp_info.set_property("cmake_target_name", f"foxtrot::{cmakename}")
        #self.cpp_info.set_property("cmake_file_name", f"{cmakename}Config.cmake")

        #we provide our own configuration files and as such disable CMakeDeps for ourselves
        self.cpp_info.set_property("cmake_find_mode", None)
        
        

    def layout(self):
        cmake_layout(self, build_folder="conanbuild")
        self.cpp.build.builddirs.append("")


def ft_version_get_req_str(verstr: str) -> str:
    vers = Version(verstr)
    cmpout = []
    for cmpin in (vers.major, vers.minor, vers.patch):
        if cmpin is not None:
            cmpout.append(str(cmpin))
        else:
            break

        reqstr = f"[^{'.'.join(cmpout)}, include_prerelease=True]"
    return reqstr

    

        
def semver_from_git_describe(gitobj) -> str:
    is_dirty = gitobj.is_dirty()
        
    try:
        last_tagged_version = gitobj.run("describe --tags --abbrev=0")
        full_desc = gitobj.run("describe --tags")
    except Exception as err:
        if is_dirty:
            return "0.0.1-dev-dirty"
        return "0.0.1-dev"
        

    verstr = semver_string_parsing_thing(last_tagged_version, full_desc, is_dirty)
    return verstr    

def semver_string_parsing_thing(last_tagged: str, full_desc: str, is_dirty: bool):
    extra_matter = full_desc.split(last_tagged)[1]
    if len(extra_matter) ==0 and not is_dirty:
        #this is an actual full tagged version
        if(last_tagged[0] == "v" or last_tagged[0] == "V"):
            return last_tagged[1:]
        return last_tagged
    elif len(extra_matter) == 0:
        #bump the patch version
        if(last_tagged[0] == "v" or last_tagged[0] == "V"):
            return last_tagged[1:] + "-dirty"
        return last_tagged + "-dirty"

    _, n_commits, other = extra_matter.split("-",2)
    n_commits = int(n_commits)

    assert other[0] == "g"
    chash = other[1:]
    
    cvers = Version(last_tagged)

    print(f"pre: {cvers.pre}")
    print(f"patch: {cvers.patch}")
    
    if cvers.pre is None:
        #bump the patch version, and add a dev string
        if cvers.patch is not None:
            if not str(cvers.patch).isnumeric():
                number, rest = string_digit_split(str(cvers.patch))
                newvers = f"{cvers.major}.{cvers.minor}.{number+1}"
            else:
                newvers = str(cvers.bump(2))
        else:
            newmajor = cvers.major
            newminor = cvers.minor if cvers.minor is not None else 0
            newpatch = 1
            newvers = str(".".join([str(_) for _ in (newmajor, newminor, newpatch)]))
    else:
        #version stays the same, we will just add a devstring
        newvers = str(cvers)

    devstr = f"+dev-{n_commits}-g{chash}"

    if cvers.pre is None:
        #the devstring is now the pre
        fullstr = f"{newvers}-{devstr}"
    else:
        fullstr = f"{newvers}{devstr}"

    if fullstr[0] == "v" or fullstr[0] == "V":
        fullstr = fullstr[1:]
    
    if is_dirty:
        return fullstr + "-dirty"

    return fullstr
        

def string_digit_split(st: str):
    gen = iter(st)
    outdigits = []
    for char in gen:
        if char.isdigit():
            outdigits.append(char)
        else:
            break

    outrest = [_ for _ in gen]
    return int("".join(outdigits)), outrest
def string_digit_split(st: str):
    gen = iter(st)
    outdigits = []
    for char in gen:
        if char.isdigit():
            outdigits.append(char)
        else:
            break

    outrest = [_ for _ in gen]
    return int("".join(outdigits)), outrest
