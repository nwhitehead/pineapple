#!/bin/bash

## What I used to get from odd 238x256 to 256x256
# convert Pineapple-256.png -gravity center -background none -extent 256x256 out.png

# in.png
cp ../images/Pineapple-256.png in.png
convert in.png -resize '256x256' out-256.png
convert in.png -resize '128x128' out-128.png
convert in.png -resize '32x32' out-32.png
convert in.png -resize '16x16' out-16.png
png2icns Pineapple.icns out-256.png out-128.png out-32.png out-16.png 
