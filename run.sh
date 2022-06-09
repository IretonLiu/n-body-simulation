#!/bin/bash

while getopts 'c:' OPTION; do
  case "$OPTION" in
    c)
      rm -rf build
      ;;
		?)
			echo invalid flags 
			;;

  esac
done
shift "$(($OPTIND -1))"

if [ ! -d "build/" ]
then
	cmake -B build
fi

echo "----- COMPILING -----"
( cd build ; make )

executables="DataGenerator Serial B"
if [ $# -eq 0 ]; then
	echo "Please supply the name of the executable"
	exit 1
fi

echo
echo "----- RUNNING -----"
executable=$1
( cd bin ; ./$executable "${@:2}") || {
	echo "Invalid executable name supplied, try one of: "
	echo $executables
}

echo
echo "----- DONE -----"
