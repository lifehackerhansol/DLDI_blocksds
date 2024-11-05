#!/bin/bash

drivers="\
    acep \
    ak2 \
    g003 \
    iply \
    m3ds \
    r4idsn \
    r4tf \
    rpgs \
    scds \
    scdssdhc \
    sg3d \
    ttio \
"

rm -rf out
mkdir -p out

for i in $drivers; do
    make PLATFORM=$i
    cp $i.dldi out
done
