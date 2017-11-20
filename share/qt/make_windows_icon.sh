#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/straks.png
ICON_DST=../../src/qt/res/icons/straks.ico
convert ${ICON_SRC} -resize 16x16 straks-16.png
convert ${ICON_SRC} -resize 32x32 straks-32.png
convert ${ICON_SRC} -resize 48x48 straks-48.png
convert straks-16.png straks-32.png straks-48.png ${ICON_DST}

