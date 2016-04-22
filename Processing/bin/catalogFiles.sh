#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Produce a catalog entry for the requested file.
#---------------------------------------------------------------------------------------------------

# Read the arguments
echo " "
echo " catalogFiles.sh  $*"
dataDir=$1
catalogDir=$2
book=$3
dataset=$4
condorOutput=$5

# Is it a skim?
skim=""
if [ "`echo $dataDir | grep cms/store/user/paus/skim`" != "" ]
then
  skim=`basename $dataDir`/
fi

# Prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book [skim: $skim]"
echo "   in directory : $dataDir"
echo "   catalog in   : $catalogDir"

mkdir -p $catalogDir/condor/$book/$dataset/$skim
script=`which catalogFile.sh`

# Create a list of files of zero length we should not look at
ZERO_LIST=`list $dataDir/$book/$dataset | grep ^0 | grep root | cut -d' ' -f2`
if [ "$ZERO_LIST" != "" ]
then
  echo ""
  echo " ZERO_LIST: list $dataDir/$book/$dataset|grep ^0 |grep root|cut -d' ' -f2"
  echo " ZERO_LIST :::: $ZERO_LIST ::::"
  for file in $ZERO_LIST
  do
    echo "  # Zero length file not considered: $file"
    echo "  remove --exe $dataDir/$book/$dataset/$file"
  done
fi

# Create a list of the files we need to catalog
echo ""
LIST=`list $dataDir/$book/$dataset| grep -v miniaod |grep -v ^0 |grep root|cut -d' ' -f2`
# make sure authentication will work

# get authentication
x509File=/tmp/x509up_u`id -u`

for file in $LIST
do
  file=`basename $file`
  logFile=`echo ${skim}$file_$$ | tr '/' '+'`
  logFile=/tmp/$logFile
  rm -f $logFile

  exists=""
  if [ "$condorOutput" != "" ]
  then
    exists=`find $catalogDir/condor/$book/$dataset/$skim -name $file.out`
  else
    if [ -d "$catalogDir/$book/$dataset/$skim" ] && \
       [ -f $catalogDir/$book/$dataset/${skim}RawFiles.00 ]
    then
      exists=`cat $catalogDir/$book/$dataset/${skim}RawFiles.?? | grep $file`
    fi
  fi

  if [ ".$exists" == "." ]
  then
    echo "  $script $dataDir/$book/$dataset $file"
    mkdir -p $catalogDir/condor/$book/$dataset/${skim}
    cat > submit_$$.cmd <<EOF
Universe                = vanilla
Requirements            = OpsysAndVer == "SL6" && Arch == "X86_64" && Disk >= DiskUsage && (Memory * 1024) >= ImageSize && HasFileTransfer
Notification            = Error
Executable              = $script
Arguments               = $dataDir/$book/$dataset $file
Rank                    = Mips
GetEnv                  = False
Input                   = /dev/null
Output                  = $catalogDir/condor/$book/$dataset/${skim}$file.out
Error                   = $catalogDir/condor/$book/$dataset/${skim}$file.err
Log                     = $catalogDir/condor/$book/$dataset/${skim}$file.log
transfer_input_files    = $x509File
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
+AccountingGroup        = "catalog.cmsprod"
Queue
EOF

    condor_submit submit_$$.cmd >& /dev/null #>& lastSub
    rm submit_$$.cmd
  fi
done

exit 0
