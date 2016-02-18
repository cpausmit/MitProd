#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Upload a list of files to dropbox.
#---------------------------------------------------------------------------------------------------
# condor base
HADOOP_BASE="/mnt/hadoop/cms"
CONDOR_UPLOAD="/home/cmsprod/cms/condor/upload"
script="$MIT_PROD_DIR/bin/uploadFiles.sh"
if [ ! -d $CONDOR_UPLOAD ]
then
  echo " No condor upload directory available. EXIT"
  exit 0
fi

# Read the arguments
echo ""
echo " uploadSample.sh  $*"
echo ""

# read command line arguments
LFN="$1"
NCOPY="$2";
REMOVE="$3";

# derive other parameters
sample=`basename $LFN`
subdir=`dirname  $LFN`
dir=`   dirname  $subdir`
book=`echo $subdir | cut -d/ -f5-10`

dn=`grid-proxy-info -subject`

# Prepare environment
echo " Process dataset: $LFN"
echo "   in directory : $subdir"
echo "   to target    : DropBox"
echo "   book         : $book"
echo "   n copy procs : $NCOPY"
echo "   remove source: $REMOVE"
echo " "

# make list of files
if [ ! -d $CONDOR_UPLOAD/$sample ]
then
  mkdir $CONDOR_UPLOAD/$sample
fi
# - done files
rm -f $CONDOR_UPLOAD/$sample/files-done.txt
touch $CONDOR_UPLOAD/$sample/files-done.txt
~/Tools/Dropbox-Uploader/dropbox_uploader.sh list /cms/$LFN|tr -s ' '|tee /tmp/${sample}-sizes.$$ \
      |cut -d ' ' -f4 > $CONDOR_UPLOAD/$sample/files-done.txt
# select the missing files from all files
rm -f $CONDOR_UPLOAD/$sample/files.txt
touch $CONDOR_UPLOAD/$sample/files.txt
all=`list  $HADOOP_BASE$LFN | grep '.root' | sed "s# #:$LFN/#"`
for lfn in $all
do

  # get the core quantities
  size=`echo $lfn | cut -d':' -f1`
  dir=`echo $lfn | cut -d':' -f2`
  file=`basename $lfn`
  dir=`dirname $dir`

  # test whether already available and get its size
  test=`grep $file $CONDOR_UPLOAD/$sample/files-done.txt`
  sizeDbx=`grep $file /tmp/${sample}-sizes.$$ | cut -d ' ' -f3`

  # go through the motions
  if [ "$test" == "" ]
  then
    echo $dir/$file >> $CONDOR_UPLOAD/$sample/files.txt
    echo $dir/$file
  elif [ "$sizeDbx" == "$size" ]
  then
    echo " EQUAL - $size  -- $file " > /dev/null
  elif [ "$sizeDbx" != "$size" ]
  then
    echo " UNEQUAL -  $size  != $sizeDbx  -- $lfn "
    echo $dir/$file >> $CONDOR_UPLOAD/$sample/files.txt
    echo $dir/$file
  fi

done

# submit NCOPY jobs to condor

# construct our job
nDoneFiles=`wc -l $CONDOR_UPLOAD/$sample/files-done.txt | cut -d ' ' -f1`
nFiles=`wc -l $CONDOR_UPLOAD/$sample/files.txt | cut -d ' ' -f1`
if   [ "$nFiles" == "" ] || [ "$nFiles" == "0" ]
then
  echo " "
  echo " No more files to upload. EXIT."

  if [ $nDoneFiles -gt 0 ] && [ "$REMOVE" == "remove" ]
  then
    echo " removeSample.sh $book $sample"    
    removeSample.sh $book $sample
  fi

  exit 0
elif [ $nFiles -lt $NCOPY ]
then
  NCOPY=$nFiles
fi

# how many files per job?
nFilesPerJob=$(( $nFiles/$NCOPY ))
nFilesTmp=$(( $nFilesPerJob*$NCOPY ))
if [ $nFilesPerJob == 1 ] && [ $nFiles -gt $NCOPY ]
then
  nFilesPerJob=2
elif [ $nFilesTmp -lt $nFiles ]
then
  nFilesPerJob=$(( $nFilesPerJob+1 ))
fi

echo ""
echo "   n files to copy: $nFiles"
echo "   n files/proc   : $nFilesPerJob"
echo ""

i=1
next=1
last=$nFilesPerJob

# make sure authentication will work
x509File=/tmp/x509up_u`id -u`

# loop over the condor jobs and submit them
while [ $i -le $NCOPY ] && [ $last -le $nFiles ]
do

  if [ $i == $NCOPY ]
  then
    last=$nFiles
  fi

  # say what we are going to submit
  echo "   uploadFiles.sh $next $last"

  # prepare the condor_submit files
  cat > submit_$$.cmd <<EOF
Universe                = vanilla
Environment             = "MIT_PROD_DIR=$MIT_PROD_DIR"
Requirements            = UidDomain == "mit.edu" && Arch == "X86_64" && OpSysMajorVer == 6 && HasFileTransfer
Notification            = Error
Executable              = $script
Arguments               = $next $last
Rank                    = Mips
GetEnv                  = True
Input                   = /dev/null
Output                  = $CONDOR_UPLOAD/$sample/upload-${next}-${last}.out
Error                   = $CONDOR_UPLOAD/$sample/upload-${next}-${last}.err
Log                     = $CONDOR_UPLOAD/$sample/upload-${next}-${last}.log
transfer_input_files    = $x509File,$MIT_PROD_DIR/bin/uploadFiles.sh,$MIT_PROD_DIR/bin/uploadFile.sh,$CONDOR_UPLOAD/$sample/files.txt
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
+AccountingGroup        = "group_cmsuser.cmsu0284"
Queue
EOF

  # submit the jobs
  condor_submit submit_$$.cmd ## >& /dev/null #>& lastSub
  rm  submit_$$.cmd

  # update counters
  next=$(( $next + $nFilesPerJob ))
  last=$(( $last + $nFilesPerJob ))
  i=$(( $i + 1 ))

done

exit 0
