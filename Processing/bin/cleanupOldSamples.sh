#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Compare samples from two software versions (old and new) and delete the old one if the new one is
# already complete. Deletions are executed based on the removeSample.sh script.
#
#                                                                      v0.0 -- C.Paus (Aug 11, 2015)
#---------------------------------------------------------------------------------------------------
MOUNT=/mnt/hadoop
BASE_LFN=/cms/store/user/paus

function usage {
  echo ""
  echo " "`basename $0`" <mitConfig> <oldVersion>  <newVersion> [ <execute> = exec, '' ] "
  echo ""
  echo "   mitConfig  - mit configuration of the samples in question. Example: filefi"
  echo "   oldVersion - version number considered to be old. Example: 041"
  echo "   newVersion - version number considered to be new. Example: 042"
  echo "   execute    - test only or exececute deletions"
  echo ""
  echo " deletions are executed based on the removeSample.sh script."
  echo ""
  exit 0
}

function deleteDecision {
  nOld="$1"
  nNew="$2"
  rc=`echo $nOld $nNew | awk '{ print ($2/$1 >= 0.8) ? 0 : 1 }'`
  echo " NEW/OLD: $nNew/$nOld -> $rc"
  return $rc            # CAREFUL: 0 -- true,  1 -- false
}

MIT_CONFIG="$1"
OLD_VERSION="$2"
NEW_VERSION="$3"
if [ "$MIT_CONFIG" == "" ] || [ "$OLD_VERSION" == "" ] || [ "$NEW_VERSION" == "" ]
then
  echo ""
  echo " Please define all required parameters."
  usage
else
  echo ""
  echo " == Cleanup config: $MIT_CONFIG  -- old: $OLD_VERSION  if available in new: $NEW_VERSION =="
  echo ""
fi
EXECUTE="$4"

list ${MOUNT}${BASE_LFN}/$MIT_CONFIG/$OLD_VERSION | cut -d ' ' -f2 | sort -u > $OLD_VERSION.old.$$
list ${MOUNT}${BASE_LFN}/$MIT_CONFIG/$NEW_VERSION | cut -d ' ' -f2 | sort -u > $NEW_VERSION.new.$$

rmList="$OLD_VERSION.old.$$ $NEW_VERSION.new.$$"

for oldSample in `cat $OLD_VERSION.old.$$`
do

  test=`grep $oldSample $NEW_VERSION.new.$$`

  if [ "$test" != "" ]
  then

    nOld=`list ${MOUNT}${BASE_LFN}/$MIT_CONFIG/$OLD_VERSION/$oldSample | wc -l`
    nNew=`list ${MOUNT}${BASE_LFN}/$MIT_CONFIG/$NEW_VERSION/$oldSample | wc -l`
    completion=`echo "$nNew / $nOld" | bc -l`
    echo " Old: $nOld  -->  New: $nNew  [ completion: $completion ]  $oldSample"

    if  deleteDecision $nOld $nNew
    then
      echo " removeSample.sh $MIT_CONFIG/$OLD_VERSION $oldSample"
      if [ "$EXECUTE" == "exec" ]
      then
        removeSample.sh $MIT_CONFIG/$OLD_VERSION $oldSample
      else
        echo " ---- not executing."
      fi
    fi

  else
    echo " Old sample not yet in new sample list: $oldSample"
  fi

done

# cleanup the temporary files
rm $rmList

exit 0
