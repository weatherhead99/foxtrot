#!/bin/bash

#check if we're actually on a bash shell
if [ ! -n "$BASH" ]
then
   echo "we seem not to be running in a bash shell, please use bash!"
   exit 1
fi



TREE_SPEC="HEAD"
if [ -n "$1" ]
then
    echo "using supplied tree spec: $1"
    TREE_SPEC=$1
    DESC=`git describe $TREE_SPEC --tags`
else
    DESC=`git describe --tags --dirty`
fi


BASHPATH=$(realpath $BASH_SOURCE)
ABS_CUR_PATH=`dirname $BASHPATH`
echo $BASHPATH
FT_SOURCE="$ABS_CUR_PATH/../"


if [ ! -n "$DESC" ]
then
    echo "invalid description, exiting"
    exit 1
fi
echo "description is $DESC"
echo "preparing archive..."
echo $FT_SOURCE
pushd $FT_SOURCE
git archive --format tar.gz --prefix "foxtrot/" -o "foxtrot-$DESC.tar.gz"  $TREE_SPEC
popd
mv $FT_SOURCE/foxtrot-$DESC.tar.gz .
