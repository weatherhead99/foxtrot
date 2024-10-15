#!/bin/bash

mkdir /foxtrot
declare -a ft_mods=("core" "protocols" "devices" "client" "server")
for mod in ${ft_mods[@]}
do
    echo "BUILDING $mod"
    mkdir -p /foxtrot_build/$mod
    cd /foxtrot_build/$mod
    cmake -GNinja /foxtrot_source/$mod -DCMAKE_INSTALL_PREFIX=/foxtrot
    ninja
    ninja install
done

