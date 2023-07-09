#!/bin/sh

usage () {
    echo "$0 usage: $0 /path file_name" && exit $1
}

if [ $1 = "-h" ] || [ $1 = "--help" ]
then
    usage 0
elif [ -z $1 ] || [ -z $2 ]
then 
    echo "arguments no specified\n" && usage 1
fi


mkdir -p "${1%/*}"
if [ ! $? -eq 0 ]
then 
    echo "unable to create directory" && exit 1
fi

if [ -d "${1%/*}" ]
then
    echo "$2" > "$1"
    if [ ! $? -eq 0 ]
    then 
        echo "unable to create file" && exit 1
    else
        echo "successfull created $1 file"
    fi
fi

exit 0