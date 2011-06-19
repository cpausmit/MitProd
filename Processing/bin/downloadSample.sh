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
onlyMissing=$1;  shift

DN=`grid-proxy-info -subject`

# Prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book"
echo "   in directory : $dataDir"
echo "   to target    : $target"
echo "   n copy procs : $nCopyProcs"
echo "   condor output: $condorOutput"
echo "   only missing : $onlyMissing"

mkdir -p         $condorOutput/$book/$dataset
makedir --exe --debug  $target/$book/$dataset
script=`which downloadFiles.sh`

# cleanup our lists and remake a clean one
#echo "rm -f $condorOutput/$book/$dataset/fileList*.txt*"
rm -f $condorOutput/$book/$dataset/fileList*.txt*

# make list of all local files
if [ "`echo $HOSTNAME | grep mit.edu`" != "" ] && \
 ( [ "`echo $dataDir | grep /castor/cern.ch`" != "" ] || \
   [ "`echo $target | grep /castor/cern.ch`" != "" ] )
then
  opt="--simple"
else
  opt=""
fi

#echo "list $opt $dataDir/$book/$dataset > $condorOutput/$book/$dataset/fileList-all.txt-bak"
list $opt $dataDir/$book/$dataset > $condorOutput/$book/$dataset/fileList-all.txt-bak

# Make sure there is a kerberos ticket available
id=`id -u`
cp /tmp/x509up_u${id} ~/.krb5/
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
if [ "`echo $HOSTNAME | grep mit.edu`" != "" ] && ( [ "`echo $dataDir | grep /castor/cern.ch`" != "" ] || [ "`echo $target | grep /castor/cern.ch`" != "" ] )
then
  opt="--simple"
else
  opt=""
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
  exists=`grep "$file" $condorOutput/$book/$dataset/fileList-done.txt`
  if [ "$exists" == "" ]
  then
    echo "   -missing-- $file with $size bytes"
    echo "$size $file" >> $condorOutput/$book/$dataset/fileList.txt
  # else
  #   echo "   -exists--- $file with $size bytes - exists"
  else
    # now check that size matches
    test=`grep "$size $file" $condorOutput/$book/$dataset/fileList-done.txt`
    if [ "$test" == "" ]
    then
      if [ "$onlyMissing" == "" ]
      then
        echo "   -fileSize- $exists (remote: $size)"
        echo "$size $file" >> $condorOutput/$book/$dataset/fileList.txt
      fi
    fi
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
# how many files per job?
nFilesPerJob=$(( $nFiles/$nCopyProcs ))
nFilesTmp=$(( $nFilesPerJob*$nCopyProcs ))
if [ $nFilesPerJob == 1 ] && [ $nFiles -gt $nCopyProcs ]
then
  nFilesPerJob=2
elif [ $nFilesTmp -lt $nFiles ]
then
  nFilesPerJob=$(( $nFilesPerJob+1 ))
fi

echo "   n files all  : $nFiles"
echo "   n files/proc : $nFilesPerJob"

i=1
next=1
last=$nFilesPerJob

# make sure condor is properly setup for us
if ! [ -z $CONDOR_LOCATION ]
then
  unset  CONDOR_LOCATION
  export CONDOR_CONFIG=/usr/local/condor/etc/condor_config
fi

# loop over the condor jobs and submit them
while [ $i -le $nCopyProcs ] && [ $last -le $nFiles ]
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
Requirements            = ( (Arch == "INTEL") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer) )
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

+AccountingGroup        = "group_cmsuser.cmsu0284"

Queue
EOF

#+x509userproxysubject   = $DN

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
