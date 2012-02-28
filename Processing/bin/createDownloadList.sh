#!/bin/bash
#===================================================================================================
# Create a list of datasets that completely define a download on the basis of a number of
# parameters. Not yet finished, the interface needs to be completed.
#
#                                                                             Ch.Paus, Aug 30, 2011
#===================================================================================================
MIT1_LOCATION="/pnfs/cmsaf.mit.edu/t2bat/cms/store/user/paus"
MIT2_LOCATION="/mnt/hadoop/cms/store/user/paus"
CERN_LOCATION="/castor/cern.ch/user/p/paus"

BOOK=$1
PATTERN=$2
version=`basename $BOOK`

SOURCE="T0_CH_CERN"

if [ "$SOURCE" == "T0_CH_CERN" ]
then
  list="list $CERN_LOCATION/$BOOK/"
else
  list="list $MIT1_LOCATION/$BOOK/ $MIT2_LOCATION"
fi
if [ "$PATTERN" != "" ]
then
  list="$list | grep $PATTERN"
fi

echo " List: $list"

$list > /tmp/fileList.$$.tmp

n=0
max=0

# counting loop
while read line ; do

  n=$((n+1))
  #echo "Line $n: $line"
  size=`echo $line | cut -d' ' -f1`
  dset=`echo $line | cut -d' ' -f2`
  #echo " Dataset - $dset (size: $size)"
  if [ ${#dset} -gt $max ]
  then
    max=${#dset}
  fi

done < /tmp/fileList.$$.tmp

# printout loop
while read line ; do

  size=`echo $line | cut -d' ' -f1`
  dset=`echo $line | cut -d' ' -f2`

  if [ "$SOURCE" == "T0_CH_CERN" ]
  then
    printf "%s %s %${max}s %s %2d %s %1d\n" \
           $CERN_LOCATION $BOOK $dset /mnt/hadoop/cms/store/user/paus 80 /home/cmsprod/download 1
  else
    printf "%s %s %${max}s %s %2d %s %1d\n" \
           $MIT1_LOCATION $BOOK $dset /mnt/hadoop/cmsprod 80 /home/cmsprod/download 1
  fi

done < /tmp/fileList.$$.tmp

rm -rf /tmp/fileList.$$.tmp
