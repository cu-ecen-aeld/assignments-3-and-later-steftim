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
elif [ ! -d $1 ] 
then 
echo "$1 is not dir\n" && usage 1
fi

FILES=$(find $1 -type f | wc -l)
LINES=$(cat $(find $1 -type f) | grep $2 | wc -l )

echo "The number of files are $FILES and the number of matching lines are $LINES"
exit 0