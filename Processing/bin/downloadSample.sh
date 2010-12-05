#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download a list of files
#---------------------------------------------------------------------------------------------------

# Read the arguments
echo ""
echo "downloadSample.sh  $*"
echo ""
dataDir=$1;      shift
book=$1;         shift
dataset=$1;      shift
target=$1;       shift
nCopyProcs=$1;   shift
condorOutput=$1; shift

# Prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book"
echo "   in directory : $dataDir"
echo "   to target    : $target"
echo "   n copy procs : $nCopyProcs"
echo "   condor output: $condorOutput"

mkdir -p $condorOutput/$book/$dataset
mkdir -p $target/$book/$dataset
script=`which downloadFiles.sh`

# cleanup our lists
rm -f $condorOutput/$book/$dataset/fileList*.txt*

var=`echo $dataDir | grep /castor/cern.ch`
if [ "$var" != "" ]
then
  storageEle="srm-cms.cern.ch"
  storagePath='/srm/managerv2?SFN='
  storageUrl="srm://${storageEle}:8443${storagePath}$dataDir/$book/$dataset"
  #srmls $storageUrl | grep root | tr -s ' ' | cut -d' ' -f 2-3 2> /dev/null 1> \
  #  $condorOutput/$book/$dataset/fileList-all.txt-bak
  list $dataDir/$book/$dataset > $condorOutput/$book/$dataset/fileList-all.txt-bak
else
  storageEle="se01.cmsaf.mit.edu"
  storagePath='/srm/managerv2?SFN='
  storageUrl="srm://${storageEle}:8443${storagePath}$dataDir/$book/$dataset"
  #echo "list $dataDir/$book/$dataset > $condorOutput/$book/$dataset/fileList-all.txt-bak"
  list $dataDir/$book/$dataset > $condorOutput/$book/$dataset/fileList-all.txt-bak
fi

# Make sure there is a kerberos ticket available
cp /tmp/x509up_u5410 ~/.krb5/
KRB5CCNAME=`klist -5 | grep 'Ticket cache:' | cut -d' ' -f 3`
if ! [ -z $KRB5CCNAME ]
then
  mkdir    -p  ~/.krb5/
  chmod 0      ~/.krb5
  chmod u=rwx  ~/.krb5
  file=`echo $KRB5CCNAME | cut -d: -f2`
  if [ -f "$file" ]
  then
    cp $file ~/.krb5/ticket
  else
    echo " ERROR -- missing kerberos ticket ($KRB5CCNAME)."
    exit 1
  fi
else
  echo " ERROR -- missing kerberos ticket ($KRB5CCNAME)."
fi

# make list of all remote files
#echo " converting all entries"
cat $condorOutput/$book/$dataset/fileList-all.txt-bak | grep root | \
while read line
do
  size=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  file=`echo $line | tr -s ' ' | cut -d ' ' -f 2`
  file=`basename $file`
  echo "$size $file" >> $condorOutput/$book/$dataset/fileList-all.txt
done

# make list of all local files
opt=''
if [ "`echo $HOSTNAME | grep mit.edu`" != "" ] && [ "`echo $dataDir | grep /castor/cern.ch`" != "" ]
then
  opt='--simple'
fi
#echo "list $opt $target/$book/$dataset | grep root > $condorOutput/$book/$dataset/fileList-done.txt"
list $opt $target/$book/$dataset | grep root > $condorOutput/$book/$dataset/fileList-done.txt

# make list of missing files
rm -f $condorOutput/$book/$dataset/fileList.txt
touch $condorOutput/$book/$dataset/fileList.txt
cat   $condorOutput/$book/$dataset/fileList-all.txt | grep root | \
while read line
do
  size=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  file=`echo $line | tr -s ' ' | cut -d ' ' -f 2`
  exists=`grep "$size $file" $condorOutput/$book/$dataset/fileList-done.txt`
  if [ "$exists" == "" ]
  then
    echo "   -missing- $file with $size bytes"
    echo "$size $file" >> $condorOutput/$book/$dataset/fileList.txt
  # else
  #   echo "   -exists-- $file with $size bytes - exists"
  fi
done

# construct our job
nFiles=`wc -l $condorOutput/$book/$dataset/fileList.txt | cut -d ' ' -f1`
if   [ "$nFiles" == "" ] || [ "$nFiles" == "0" ]
then
  echo " "
  echo " No more files to download. EXIT."
  exit 0
elif [ $nFiles -lt $nCopyProcs ]
then
  nCopyProcs=$nFiles
fi
nFilesPerJob=$(( $nFiles/$nCopyProcs ))
echo "   n files all  : $nFiles"
echo "   n files/proc : $nFilesPerJob"

i=1
next=1
last=$nFilesPerJob

while [ $i -le $nCopyProcs ]
do
  if [ $i == $nCopyProcs ]
  then
    last=$nFiles
  fi

  # say what we are going to submit
  echo "   downloadFiles.sh $dataDir $book $dataset $target $condorOutput $next $last"

  logFile=`echo download:$book/$dataset/${next}-${last}.txt | tr '/' '+'`
  logFile=/tmp/$logFile
  rm -f $logFile

  # prepare the condor_submit files
  cat > submit_$$.cmd <<EOF
Universe                = vanilla
Requirements            = ( (Arch == "X86_64" || Arch == "INTEL") && (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer) )
Notify_user             = paus@mit.edu
Notification            = Error
Executable              = $script
Arguments               = $dataDir $book $dataset $target $condorOutput $next $last
Rank                    = Mips
GetEnv                  = True
Input                   = /dev/null
Output                  = $condorOutput/$book/$dataset/${next}-${last}.out
Error                   = $condorOutput/$book/$dataset/${next}-${last}.err
Log                     = $logFile
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
Queue
EOF

  # submit the jobs
  condor_submit submit_$$.cmd >& /dev/null #>& lastSub
  #cat submit_$$.cmd
  rm  submit_$$.cmd

  # update counters
  next=$(( $next + $nFilesPerJob ))
  last=$(( $last + $nFilesPerJob ))
  i=$(( $i + 1 ))
done

exit 0
