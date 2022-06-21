#!/bin/bash

isMPI=false
while getopts 'cm' OPTION; do
  case "$OPTION" in
		m)
			isMPI=true
			;;
    c)
      rm -rf build
      ;;
		?)
			echo invalid flags 
			;;

  esac
done
shift "$(($OPTIND -1))"

# if [ ! -d "build/" ]
# then
# 	cmake -B build
# fi

# echo "----- COMPILING -----"
# ( cd build ; make )

executables="DataGenerator Serial Cuda MPI Validate"
if [ $# -eq 0 ]; then
	echo "Please supply the name of the executable"
	exit 1
fi

echo
echo "----- RUNNING -----"

executable=$1
if [ $isMPI = true ]
then
	( cd bin ; mpiexec -n $2 ./$executable "${@:3}") || {
	echo "Invalid executable name supplied, try one of: "
	echo $executables
	}
else
	( cd bin ; ./$executable "${@:2}") || {
		echo "Invalid executable name supplied, try one of: "
		echo $executables
	}
fi

echo
echo "----- DONE -----"
