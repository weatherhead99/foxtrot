from conans.client import conan_api
import os

api, cache, userio = conan_api.Conan.factory()

user="weatherhead99"
channel="testing"


buildtools_version = "0.1"
print("building python utils for other conan packages...")
api.export(".", name="FoxtrotBuildUtils", user=user, channel=channel, 
           version=buildtools_version)


print("building foxtrot core conan package...")
api.create("../core", user=user, channel=channel)

print("building foxtrot server conan package...")
api.create("../server", user=user, channel=channel)