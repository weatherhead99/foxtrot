from conans.client import conan_api
import os

api, cache, userio = conan_api.Conan.factory()

user="weatherhead99"
channel="testing"


buildtools_version = "0.1"
print("building python utils for other conan packages...")
api.export(".", name="FoxtrotBuildUtils", user=user, channel=channel, 
           version=buildtools_version)


createfun = lambda s : api.create(s, user=user, channel=channel, keep_source=True,
                                  keep_build=True, update=True)

print("building foxtrot core conan package...")
createfun("../core")

print("building foxtrot server conan package...")
createfun("../server")
