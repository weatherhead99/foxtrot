from conan.internal.deploy import full_deploy
from conan.tools.files import copy
import os


def copy_setups(conanfile, output_folder):
    if hasattr(conanfile, "deploy_setups"):
        print(f"deploying setup files: {conanfile.deploy_setups}")
        dirs = [conanfile.build_folder, conanfile.package_folder]
        for fl in conanfile.deploy_setups:
            for dr in (_ for _ in dirs if _ is not None):
                print(f"deploying from {dr}")
                pattern = f"*{fl}"
                copy(conanfile, pattern, dr, output_folder, keep_path=False)

def deploy(graph, output_folder, **kwargs):
    #first, run a full deploy
    full_deploy(graph, output_folder)
    print("------FOXTROT SERVER DEPLOYER-------")
    for req, cfile in graph.root.conanfile.dependencies.items():
        if "foxtrot_server" in req.ref.name:
            bindir = cfile.cpp_info.bindirs[0]
            copy(cfile, "exptserve", bindir, output_folder)
        print(cfile._conanfile)
        print(hasattr(cfile._conanfile, "deploy_setups"))
        copy_setups(cfile._conanfile, output_folder)
