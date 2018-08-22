#!/bin/bash
BUILD_TYPE=$1
if [ "$BUILD_TYPE" = "" ];
then 
  echo "build type not specified, choosing Release"
  BUILD_TYPE=Release
fi
echo "build type: $BUILD_TYPE"
echo "installing conan..."
pip install conan
echo "adding conan remotes..."
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan 
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan remote add weatherhead99 https://api.bintray.com/conan/weatherhead99/conan_packages 
mkdir ../conanbuild
cd ../conanbuild
echo "installing conan dependencies..."
conan install .. --build=missing -sbuild_type=$BUILD_TYPE
echo "running cmake..."
cmake .. -DBUILD_DASHBOARD=OFF