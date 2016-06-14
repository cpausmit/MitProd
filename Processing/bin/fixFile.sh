#!/bin/bash
#===================================================================================================
# Assume a local files is found to be broken. Go through a full recovery:
#  + check they are really broken
#  + check they are not broken on Tier-2
#  + remove broken file from Tier-3 and download the proper one from Tier-2
#  + in case also Tier-2 is broken check CERN and download even back to Tier-2
#===================================================================================================
MIT_LOC="/mnt/hadoop/cms/store/user/paus"
CERN_LOC="/castor/cern.ch/user/p/paus"

FILE="$1"
FLAG="$2"

fullFile="$FILE"
file=`basename $fullFile`
dir=`dirname $fullFile`
dset=`basename $dir`
tmp=`dirname $dir`
version=`basename $tmp`
tmp=`dirname $tmp`
cfg=`basename $tmp`

test=`list $dir/$file 2> /dev/null`
if [ "$test" != "" ]
then
  echo ""
  echo " TEST - File exists, check integrity."
  echo "        $dir/$file"
  test=`catalogFile.sh $dir $file 2> /dev/null | grep XX-CATALOG-XX`
  if [ "$test" != "" ]
  then
    echo " STOP - File seems to be fine?! Leave it."
    echo "        $dir/$file"
    echo ""
    exit 0
  else
    echo " File is really broken. Remove it."
    remove --catalog $dir/$file
  fi
else
  echo ""
  echo " TEST - File does not exist."
  echo "        $dir/$file"
fi

# once we arrive here, the file is gone and we are recovering

echo " Start recovery from Tier-2."
test=`catalogFile.sh $MIT_LOC/$cfg/$version/$dset $file 2> /dev/null | grep XX-CATALOG-XX`
if [ "$test" != "" ]
then
  echo ""
  echo " GOOD - File seems to be fine on Tier-2. Grab it."
  #echo "downloadFile.sh $MIT_LOC/$cfg/$version/$dset/$file $dir/$file"
  downloadFile.sh $MIT_LOC/$cfg/$version/$dset/$file $dir/$file >& /dev/null 
  echo "        -> Download status: $?"
  test=`catalogFile.sh $dir $file 2> /dev/null | grep XX-CATALOG-XX`
  if [ "$test" != "" ]
  then
    echo ""
    echo " GOOD - Recovery completed successfully."
    echo "        $dir/$file"
    echo ""
  else
    echo " ERRO - File is broken again. Removing it."
    echo "        $dir/$file"
    echo ""
    remove --catalog $dir/$file
  fi
else
  echo ""
  echo " STOP - File seems to be broken on Tier-2."
  # in case both Tier-2 and Tier-3 broken and requested removed both
  if [ "$FLAG" == "remove" ]
  then
    remove --catalog $dir/$file 
    remove --catalog $MIT_LOC/$cfg/$version/$dset/$file
  fi
  echo " Start recovery from CERN."
  test=`list $CERN_LOC/$cfg/$version/$dset/$file 2> /dev/null`
  if [ "$test" != "" ]
  then
    echo ""
    echo " GOOD - File seems to be available at CERN. Grab it."
    downloadFile.sh $CERN_LOC/$cfg/$version/$dset/$file $dir/$file >& /dev/null 
    echo "        -> Download status: $?"
    test=`catalogFile.sh $dir $file 2> /dev/null | grep XX-CATALOG-XX`
    if [ "$test" != "" ]
    then
      echo ""
      echo " GOOD - Recovery completed successfully from CERN."
      echo "        $dir/$file"
      # try to recover Tier-2 from CERN if that is required
      if [ "$MIT_LOC/$cfg/$version/$dset/$file" != "$dir/$file" ]
      then
        echo " Start recovery on Tier-2."
        downloadFile.sh $CERN_LOC/$cfg/$version/$dset/$file $MIT_LOC/$cfg/$version/$dset/$file \
                        >& /dev/null 
        echo "        -> Download (to Tier-2) status: $?"
        test=`catalogFile.sh $MIT_LOC/$cfg/$version/$dset $file 2> /dev/null | grep XX-CATALOG-XX`
        if [ "$test" != "" ]
        then
          echo ""
          echo " GOOD - Recovery on Tier-2 completed successfully from CERN."
          echo "        $MIT_LOC/$cfg/$version/$dset/$file"
          echo ""
        else
          echo " ERRO - File on tier-2 is still broken. Stop process"
          echo "        $MIT_LOC/$cfg/$version/$dset/$file"
          echo ""
        fi    
      fi
    else
      echo " ERRO - File is broken again. Removing it."
      echo "        $dir/$file"
      echo ""
      rm -f  $dir/$file
    fi
  else
    echo ""
    echo " STOP - File seems not to be available at CERN."
    echo ""
  fi
fi 

exit 0
