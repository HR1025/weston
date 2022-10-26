#!/bin/bash

# 1 - configure (may be you can close it when you debug)
rm -rf build
meson build \
-Dcolor-management-lcms=false \
-Dlauncher-logind=false \
-Dbackend-drm-screencast-vaapi=false \
-Ddeprecated-wl-shell=true \
-Dbackend-rdp=false \
-Dbackend-x11=false \
-Dxwayland=false \
-Dcolor-management-colord=false \
-Dsystemd=false \
-Dremoting=false \
-Dpipewire=false \
-Dbackend-custom-dump=false 
# 2 - building
ninja -C build
# 3 - packing
rm -rf install
mkdir install
# 3.1 -  copy dynamic lib
soFiles="$(find ./build -name "*.so*")"
for file in $soFiles
do
    if [ -f "${file}" ]; then
        # echo "copy file from ${file} to ./install/$(basename "$file")"
        cp "${file}" "./install/$(basename "$file")"
    fi
done
# 3.2 - cpoy exe
cp ./build/compositor/weston ./install/weston
# 3.3 - copy plugin
cp ./build/clients/weston-keyboard ./install/weston-keyboard
cp ./build/clients/weston-desktop-shell ./install/weston-desktop-shell
cp ./build/clients/weston-terminal ./install/weston-terminal
# 3.4 - copy resource
cp -rf ./data ./install 
# 3.5 - copy config
cp ./weston.ini ./install/weston.ini