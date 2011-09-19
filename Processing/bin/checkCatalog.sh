#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Produce a catalog entry for the requested file.
#---------------------------------------------------------------------------------------------------

# Read the arguments
#echo " "
#echo "checkCatalog.sh  $*"
dataDir=$1
catalogDir=$2
book=$3
dataset=$4

nFilesPerSet=20

# Prepare environment
echo " "
echo " Checking dataset: $dataset  of book: $book"
echo "   in catalog    : $catalogDir"
echo " "

## Make backup copy
#cat $catalogDir/$book/$dataset/RawFiles.?? |sort -u > $catalogDir/$book/$dataset/$$.RawFiles

# Fix needed?
fix=0
# Create a list of the files we have already cataloged
CLIST=`cat $catalogDir/$book/$dataset/RawFiles.?? |cut -d ' ' -f1 | sort -u`
# Create a list of the files we have presently on disk
DLIST=`list $dataDir/$book/$dataset|grep -v ^0 |grep root|cut -d' ' -f2 > /tmp/$$.dlist`
for file in $CLIST
do
  file=`basename $file`  #;echo $file
  exists=`grep $file /tmp/$$.dlist`
  if [ ".$exists" == "." ]
  then
    fix=1
    # Say an inconsistent file was found and fix is on its way
    echo " File in catalog but not on disk: $file -> fixing raw information"
    cat $catalogDir/$book/$dataset/RawFiles.?? | sort -u | grep -v $file \
      > /tmp/RawFiles.$$
    if [ "$?" == "0" ]
    then
      rm $catalogDir/$book/$dataset/RawFiles.??
      mv /tmp/RawFiles.$$ $catalogDir/$book/$dataset/RawFiles.00
    else
      echo "Could not fix the catalog. Writing to  /tmp/RawFiles.$$ failed"
    fi
  fi

done
rm -f /tmp/$$.dlist

if [ "$fix" != "0" ]
then
  echo " -> regenerate the catalog"
  generateCatalog.py --nFilesPerSet=$nFilesPerSet --rawFile=$catalogDir/$book/$dataset
fi

exit;
