#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Find old samples by comparing an old release with a newer release.
#
#---------------------------------------------------------------------------------------------------
domain=`echo $HOSTNAME | cut -d'.' -f2-100`
CATALOG=~cmsprod/catalog

LOCATION="/mnt/hadoop/cms/store/user/paus"

klist -s
if [ $? != 0 ]; then
  kinit -f
fi

BOOK_OLD="$1"
BOOK_NEW="$2"

echo ""
echo "Trying to find old samples that exist in newer release"
echo "  OLD --> $BOOK_OLD"
echo "  NEW --> $BOOK_NEW"

newList=`rglexec ls -1 $LOCATION/$BOOK_NEW`

for oldSample in `glexec ls -1 $LOCATION/$BOOK_OLD`
do
  for newSample in $newList
  do
    if [ "$newSample" == "$oldSample" ]
    then
      echo "## Delete old sample ($oldSample) because new exists ($newSample)"
      echo " removeSample $BOOK_OLD $oldSample"
    fi
  done
done

exit 0
