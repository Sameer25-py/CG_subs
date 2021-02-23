#!/usr/bin/env bash

echo -n "Enter your 8-digit LUMS ID (e.g. 21100053): "
read ROLL

ZIP_NAME=${ROLL}".zip"

zip ${ZIP_NAME} \
  src/*.{cpp,h} \
  src/static_scene/*.{cpp,h}
zip -r ${ZIP_NAME} website

printf "\033[93mCreated a new file '${ZIP_NAME}' in this directory. "
printf "SUBMIT THIS ZIP FILE ON LMS\n"
