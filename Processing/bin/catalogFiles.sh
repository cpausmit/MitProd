#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Produce a catalog entry for the requested file.
#---------------------------------------------------------------------------------------------------

# Read the arguments
echo " "
echo "catalogFiles.sh  $*"
dataDir=$1
catalogDir=$2
book=$3
dataset=$4
condorOutput=$5

# Prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book"
echo "   in directory : $dataDir"
echo "   catalog in   : $catalogDir"

mkdir -p $catalogDir/condor/$book/$dataset
script=`which catalogFile.sh`

var=`echo $dataDir | grep castor/cern.ch`
if [ "$var" != "" ]
then
  echo " "
  echo " Staging files into castor in $dataDir/$book/$dataset"
  echo "   -- >> switched OFF for the moment << -- "
  # $HOME/cms/jobs/bin/stageSample.py --dataDir=$dataDir/$book/$dataset
  # sleep 4;
#else
  #echo " "
  #echo " No staging, files are on disk!"
  #sleep 2;
fi

# Create a list of files of zero length we should not look at
set LIST=`list $dataDir/$book/$dataset | grep ^0 | grep root | cut -d' ' -f2`

# Indicate zero-length files
for file in $LIST
do
  echo "  # Zero length file not considered: $file"
  echo "  remove --exe $dataDir/$book/$dataset/$file"
done

# Create a list of the files we need to catalog
echo ""
LIST=`list $dataDir/$book/$dataset|grep -v ^0 |grep root|cut -d' ' -f2`
## echo LIST :::: $LIST ::::

# Make sure there is a kerberos and globus tickets available
mkdir    -p  ~/.krb5/
chmod 0      ~/.krb5
chmod u=rwx  ~/.krb5
cp /tmp/x509up_u`id -u` ~/.krb5/
if ! [ -z $KRB5CCNAME ]
then
  file=`echo $KRB5CCNAME | cut -d: -f2`
  if [ -e "$file" ]
  then
    cp $file ~/.krb5/ticket
  else
    echo " ERROR -- missing kerberos ticket ($KRB5CCNAME)."
    exit 1
  fi
fi

for file in $LIST
do
  file=`basename $file`  #;echo $file
  logFile=`echo $book/$dataset/$file | tr '/' '+'`
  logFile=/tmp/$logFile
  rm -f $logFile

  exists=""
  if [ "$condorOutput" != "" ]
  then
    exists=`find $catalogDir/condor/$book/$dataset -name $file.out`
  else
    if [ -d "$catalogDir/$book/$dataset" ] && [ -f $catalogDir/$book/$dataset/RawFiles.00 ]
    then
      exists=`cat $catalogDir/$book/$dataset/RawFiles.?? | grep $file`
    fi
  fi

  #echo EXISTS: $exists

  if [ ".$exists" == "." ]
  then
    echo "  $script $dataDir/$book/$dataset $file"
    cat > submit_$$.cmd <<EOF
Universe                = vanilla
Requirements            = (MACHINE != "dtfnal01.cern.ch") && ( (Arch == "X86_64" || Arch == "INTEL") && (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer) )
Notify_user             = paus@mit.edu
Notification            = Error
Executable              = $script
Arguments               = $dataDir/$book/$dataset $file
Rank                    = Mips
GetEnv                  = True
Input                   = /dev/null
Output                  = $catalogDir/condor/$book/$dataset/$file.out
Error                   = $catalogDir/condor/$book/$dataset/$file.err
Log                     = $logFile
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
+AccountingGroup = "catalog.cmsprod"
Queue
EOF

    condor_submit submit_$$.cmd >& /dev/null #>& lastSub
    rm submit_$$.cmd
  fi
done

exit 0


## Requirements            = (Arch == "X86_64" || Arch == "INTEL") && (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer)
