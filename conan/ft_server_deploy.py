from conan.internal.deploy import full_deploy
from conan.tools.files import copy
import os

SCRIPT_BASE = """\
#!/bin/bash
export dir="{DIRPATH}"
source ${{dir}}/conanrun.sh
${{dir}}/exptserve {SETUP} "$@"
"""


def generate_script(output_folder, script_name: str, setup_name: str):
    pth = os.path.join(output_folder, script_name)
    with open(pth, "w") as f:
        script = SCRIPT_BASE.format(DIRPATH=output_folder, SETUP=setup_name)
        f.write(script)
    os.chmod(pth,0o744)
    


def copy_setups(conanfile, output_folder):
    allsetups = []
    if hasattr(conanfile, "deploy_setups"):
        print(f"deploying setup files: {conanfile.deploy_setups}")
        if isinstance(conanfile.deploy_setups, str):
            setups = (conanfile.deploy_setups,)
        else:
            setups = conanfile.deploy_setups
        dirs = [conanfile.build_folder, conanfile.package_folder]
        print(type(conanfile.deploy_setups))
        for fl in setups:
            print(fl)
            allsetups.append(fl)
            for dr in (_ for _ in dirs if _ is not None):
                print(f"deploying from {dr}")
                pattern = f"*{fl}"
                copy(conanfile, pattern, dr, output_folder, keep_path=False)
    return allsetups

def deploy(graph, output_folder, **kwargs):
    #first, run a full deploy
    full_deploy(graph, output_folder)
    print("------FOXTROT SERVER DEPLOYER-------")
    allsetups = []
    for req, cfile in graph.root.conanfile.dependencies.items():
        if "foxtrot_server" in req.ref.name:
            bindir = cfile.cpp_info.bindirs[0]
            copy(cfile, "exptserve", bindir, output_folder)

#        print(cfile._conanfile)
#        print(hasattr(cfile._conanfile, "deploy_setups"))
        allsetups.extend(copy_setups(cfile._conanfile, output_folder))

    print(allsetups)
    if len(allsetups) == 1:
        generate_script(output_folder, "runexptserve.sh", allsetups[0])
    elif len(allsetups) > 1:
        for setup in allsetups:
            basename = setup.split(".")[0]
            scriptname = f"runexptserve_{basename}.sh"
            generate_script(output_folder, scriptname, setup)
