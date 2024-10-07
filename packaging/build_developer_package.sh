#!/bin/bash



if [[ -z "${FT_BUILD_UNATTENDED}" ]]; then
    echo "running FT developer package build in interactive mode"
    read -p "This will remove all existing foxtrot packages in your conan cache, are you sure (Y/N)?" -n 1 -r
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
	echo "exiting..."
	exit 1 || return 1
    fi   
else
    echo "running FT developer package build unattended"
fi




export_packages() {
    echo "exporting conan packages"
    conan export ${1}/core/
    conan export ${1}/protocols/
    conan export ${1}/devices/
    conan export ${1}/server/
    conan export ${1}/client/
}

