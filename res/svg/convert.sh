#!/bin/bash

for f in *.svg; do
  rsvg-convert -o png/$f.png -h 800 -w 800 $f
done
