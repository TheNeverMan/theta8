#!/bin/bash
./transcoder.awk "$1" > "$1.tmp"
ppmtobmp "$1.tmp" > "$1.tmp.bmp"
bmptoppm "$1.tmp.bmp" > "$1.ppm"
rm -f "$1.tmp"
rm -f "$1.tmp.bmp"
