import argparse
from conan.api.conan_api import ConanAPI
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
    parser.add_argument("-c", "--clean", action="store_true")
    args = parser.parse_args()

    if args.clean:
        print("cleaning foxtrot packages...")
        remove_all_recipes(capi, "foxtrotbuildutils/*")
        remove_all_recipes(capi, "foxtrot_*/*")

    for pkg in ft_modules:
        conanfile = os.path.join("..", pkg, "conanfile.py")
        print(f" exporting: {pkg}")
        capi.export.export(conanfile, None, None, None, None)


    


if __name__ == "__main__":
    main()
    
