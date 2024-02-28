import argparse
from conan.api.conan_api import ConanAPI
from conan.cli.commands.export import common_args_export
from conan.cli.args import add_common_install_arguments
import os

ft_modules = ["core", "protocols", "devices", "server"]

def remove_all_recipes(api, searchterm: str):
    recipes = api.search.recipes(searchterm)
    revs = [api.list.recipe_revisions(_) for _ in recipes]

    for rev in revs:
        print(f" removeing: {rev}")
        api.remove.all_recipe_revisions(rev)
        api.remove.recipe(rev)


def main():
    capi = ConanAPI()

    parser = argparse.ArgumentParser(prog="build_conan_packages")
    parser.add_argument("-c", "--clean", action="store_true", help="clean out existing foxtrot packages")
    parser.add_argument("-p", "--profile", help="conan profile to use, defaults to default",
                        default="default", type=str)
    parser.add_argument("-e", "--editable", help="build packages in local editable mode, rather than as conan packages in cache", action="store_true")
    parser.add_argument("-se", "--server-editable", help="build server package in editable mode but all others in cache package mode", action="store_true")

    parser.add_argument("-u", "--update", help="update remote dependencies")

    args = parser.parse_args()

    if args.clean:
        print("cleaning foxtrot packages...")
        remove_all_recipes(capi, "foxtrotbuildutils/*")
        remove_all_recipes(capi, "foxtrot_*/*")

    profile = capi.profiles.get_profile([args.profile])
        
    pkgrefs = []

    for pkg in ft_modules:
        fpath, fname = os.path.split(__file__)
        conanfile = os.path.join(fpath, "..", pkg, "conanfile.py")
        print(f" exporting: {pkg}")
        ref, conanfile = capi.export.export(conanfile, None, None, None, None)
        print(ref)
        print(type(ref))
        pkgrefs.append(ref)

    #for now let's just build a server
    serverref = [_ for _ in pkgrefs if "foxtrot_server" in _.name]
    deps_graph = capi.graph.load_graph_requires(serverref,
                                                tool_requires = [],
                                                lockfile=None,
                                                remotes=None,
                                                update=args.update,
                                                profile_build = profile,
                                                profile_host = profile)
    

    deps_graph.report_graph_error()
    capi.graph.analyze_binaries(deps_graph, ["missing"], remotes=capi.remotes.list(),
                                update=args.update, lockfile=[])

    capi.install.install_binaries(deps_graph=deps_graph, remotes=capi.remotes.list())
    
if __name__ == "__main__":
    main()

