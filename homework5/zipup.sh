#!/usr/bin/env bash

echo -n "Enter your roll number: "
read ROLLNUMBER

ZIP_NAME="$(basename "$(pwd)")""_"${ROLLNUMBER}".zip"

zip ${ZIP_NAME} \
  src/*.{cpp,h} \
zip -r ${ZIP_NAME} website

printf "\033[93mCreated a new file '${ZIP_NAME}' in your project directory.\n"
printf "You're not done yet! Go to LMS to submit your assignment for CS452."
