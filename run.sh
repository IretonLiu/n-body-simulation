#!/bin/bash

while getopts 'ce:' OPTION; do
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

( cd build ; make )

executables="DataGenerator A B"
if [ $# -eq 0 ]; then
	echo "Please supply the name of the executable"
	exit 1
fi

executable=$1
( cd bin ; ./$executable) || {
	echo "Invalid executable name supplied, try one of: "
	echo $executables
}
