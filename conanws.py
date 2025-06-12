from conan import Workspace
from conan.tools.scm import Git
import os
import runpy
import sys


_conan_base_package_dirs = ["conan", "core", "protocols", "devices", "client", "server"]

class MyWorkspace(Workspace):

   def name(self):
      return "foxtrot"

   def _get_versioning_function(self):
      mod = runpy.run_path(os.path.join(self.folder, "conan", "conanfile.py"))
      return mod["semver_from_git_describe"]

   def packages(self):
      versfun  = self._get_versioning_function()

      if hasattr(self, "_precalc_packages"):
         return self._precalc_packages

      out = {}
      for pkgdir in _conan_base_package_dirs:
         conanfile = self.load_conanfile(os.path.join(self.folder, pkgdir))
         self.output.info(f"workspace package name: {conanfile.name}")
         if (vers := conanfile.version) is None:
            gitobj = Git(conanfile, conanfile.recipe_folder)
            vers = versfun(gitobj)
            self.output.info(f"workspace version (calculated): {vers}")
         else:
            self.output.info(f"workspace version: {conanfile.version}")

         out[f"{conanfile.name}/{vers}"] = {"path" : pkgdir}

      self._precalc_packages = out
      return out

