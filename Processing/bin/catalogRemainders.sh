#!/bin/bash
LOCATION="/cms/store/user/paus"
BOOK=$1
FLAG=$2

version=`basename $BOOK`
first=1

echo " LIST : t2tools.py --action ls --source /cms/store/user/paus/filefi/044/* | grep crab_0| cut -d ' ' -f2"

# make list of all files
t2tools.py --action ls --source /cms/store/user/paus/filefi/044/*/crab_0*/ | grep _tmp > /tmp/catalog_done.$$

for line in `t2tools.py --action ls --source /cms/store/user/paus/filefi/044/* | grep crab_0 | cut -d ' ' -f2`
do

  dir=`echo $line | cut -d ' ' -f 2`
  obsDir=$dir
  remove="0"
  crabId=`basename $dir`
  dir=`dirname $dir`
  dataset=`basename $dir`
  extDataset="$dataset/$crabId"
  first=1

  echo "catalog.sh -ceg $version $extDataset $FLAG"

  rootFiles=`grep $extDataset /tmp/catalog_done.$$ | grep _tmp.root`

  if [ "$rootFiles" != "" ]
  then
    echo ' INFO -- found root files catalog them.'
    catalog.sh -ceg $version $extDataset $FLAG
  else
    echo ' INFO -- no root files found, moving on.'
  fi

#  if [ "$first" == 1 ] && [ "`echo $line | grep root`" != "" ]
#  then
#    echo "catalog.sh -ceg $version $extDataset --remove"
#    catalog.sh -ceg $version $extDataset       --remove
#    first=0
#  fi 
#
#  if [ "$remove" == "0" ] && [ "$FLAG" == "remove" ]
#  then
#    echo "removing: $obsDir"
#    glexec rmdir $obsDir
#    remove="1"
#  fi

done

rm /tmp/catalog_done.$$

exit 0
