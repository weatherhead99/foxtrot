#!/bin/bash

TREE_SPEC="HEAD"
if [ -n "$1" ]
then
    echo "using supplied tree spec: $1"
    TREE_SPEC=$1
fi

FT_SOURCE="$BASH_SOURCE/../"
DESC=`git describe $TREE_SPEC --tags`
if [ ! -n "$DESC" ]
then
    echo "invalid description, exiting"
    exit 1
fi
echo "description is $DESC"
echo "preparing archive..."
git archive $DESC
