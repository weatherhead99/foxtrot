set BUILD_TYPE=%1
ECHO "build type: %BUILD_TYPE%"
ECHO "setting path to include local python scripts..."
set PATH=%PATH%;%APPDATA%\Python\Python36\scripts
set VCVARS_BAT=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat
ECHO "installing conan..."
pip install --user conan > NUL
ECHO "adding conan remotes..."
conan remote add inexorgame https://api.bintray.com/conan/inexorgame/inexor-conan 
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan remote add weatherhead99 https://api.bintray.com/conan/weatherhead99/conan_packages 
ECHO "setting up build environment
%VCVARS_BAT%
ECHO "installing conan based dependencies for foxtrot..."
cd %FT_DIR%
mkdir ..\conanbuild
cd ..\conanbuild
conan install .. -sbuild_type=%BUILD_TYPE%
ECHO "running cmake..."
cmake .. -A x64 -Thost=x64 -DBUILD_DASHBOARD=OFF 
ECHO "building foxtrot in RELEASE mode..."
cmake --build . --config %BUILD_TYPE%