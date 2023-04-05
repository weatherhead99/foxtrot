from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps
from conan.tools.scm import Git, Version
from conan.tools.files import load, update_conandata
from conan.tools.env import Environment, VirtualRunEnv, VirtualBuildEnv
from conan.tools.files import  collect_libs
import os

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

            basepkg = [_.__name__ for _ in bases].index(cls.BASE_PKG_NAME)
        
            n.options = getattr(n, "options", {}) | bases[basepkg].options
            n.default_options = getattr(n, "default_options", {}) | bases[basepkg].default_options

        return n

def ft_require(conanfile, substr: str) -> None:
    if getattr(conanfile, "stable_release", False):
        conanfile.requires(f"foxtrot_{substr}/[~{conanfile.version}]@{conanfile.user}/{conanfile.channel}")
    else:
        conanfile.requires(f"foxtrot_{substr}/[~{conanfile.version},include_prerelease=True]@{conanfile.user}/{conanfile.channel}")
    
class FoxtrotBuildUtils(ConanFile):
    name = "foxtrotbuildutils"
    version = "0.4.0"
    default_user = "weatherill"
    default_channel = "stable"

class FoxtrotCppPackage(metaclass=FoxtrotCppMeta):
    default_user = "weatherill"
    default_channel = "stable"
    homepage = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    url = "https://gitlab.physics.ox.ac.uk/OPMD_LSST/foxtrot"
    author = "Dan Weatherill (daniel.weatherill@physics.ox.ac.uk)"
    settings = "os", "compiler", "build_type", "arch"
    license = "UNLICENSED"
    options = {"silent_build" : [True, False]}
    default_options = {"silent_build" : True}
    
    def export(self):
        git = Git(self, self.recipe_folder)
        if git.is_dirty():
            self.output.info("git repo is dirty, local create command will not capture SCM info")
            return
        scm_url, scm_commit = git.get_url_and_commit()
        update_conandata(self, {"sources" : {"commit" : scm_commit, "url" : scm_url}})


    def set_version(self):
        git = Git(self, self.recipe_folder)
        self.version = semver_from_git_describe(git)
            
    def build(self):
        cmake = CMake(self)
        #remove to make new CMake helper error happy
        cmake.configure()
        env_build = RunEnvironment(self)
        with environment_append(env_build.vars):
            cmake.build()
        cmake.install()
            
    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.libdirs = ["lib/foxtrot"]
        self.cpp_info.names["cmake_find_package"] = "foxtrot"
        self.cpp_info.builddirs.append("lib")

    def generate(self):
        buildenv = VirtualBuildEnv(self)
        buildenv.generate()

        deps = CMakeDeps(self)
        deps.generate()

        
        tc = CMakeToolchain(self)
        tc.variables["VERSION_FROM_CONAN"] = self.version
        if self.options["silent_build"]:
            tc.variables["CONAN_CMAKE_SILENT_OUTPUT"] = True
        tc.generate()

    def fix_cmake_def_names(self, cmakename: str):
        self.cpp_info.names["cmake_find_package"] = cmakename
        self.cpp_info.names["cmake_find_package_multi"] = cmakename
        self.cpp_info.builddirs.append("lib/cmake/%s" % cmakename)

    def package_id(self):
        pass
        #silent build doest not affect binary output
        del self.info.options.silent_build
 

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
            newvers = str(Version(newmajor, newminor, newpatch))
    else:
        #version stays the same, we will just add a devstring
        newvers = str(cvers)

    devstr = f"dev.{n_commits}+{chash}"

    if cvers.pre is None:
        #the devstring is now the pre
        fullstr = f"{newvers}-{devstr}"
    else:
        fullstr = f"{newvers}.{devstr}"

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
