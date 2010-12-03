#!/bin/tcsh -f
#---------------------------------------------------------------------------------------------------
# Produce a catalog entry for the requested file.
#---------------------------------------------------------------------------------------------------

# Read the arguments
echo "\ncatalogFiles.csh  $*"
set dataDir=$1
set catalogDir=$2
set book=$3
set dataset=$4
set condorOutput=$5

# Prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book"
echo "   in directory : $dataDir"
echo "   catalog in   : $catalogDir"

mkdir -p $catalogDir/condor/$book/$dataset
set script=`which catalogFile.sh`

set var=`echo $dataDir | grep castor/cern.ch`
if ( "$var" != "" ) then
  echo " "
  echo " Staging files into castor in $dataDir/$book/$dataset"
  echo "   -- >> switched OFF for the moment << -- "
  # $HOME/cms/jobs/bin/stageSample.py --dataDir=$dataDir/$book/$dataset
  # sleep 4;
else
  #echo " "
  #echo " No staging, files are on disk!"
  #sleep 2;
endif 

# Create a list of files of zero length we should not look at
set LIST=`list $dataDir/$book/$dataset | grep ^0 | grep root | cut -d' ' -f2`

# Indicate zero-length files
foreach file ($LIST)
  echo "  # Zero length file not considered: $file"
  echo "  castorRmRf.py --exe --pattern=$file $dataDir/$book/$dataset"
end

# Create a list of the files we need to catalog
echo ""
set LIST=`list $dataDir/$book/$dataset|grep -v ^0 |grep root|cut -d' ' -f2`
## echo LIST :::: $LIST ::::

# Make sure there is a kerberos ticket available
if ( $?KRB5CCNAME ) then
  mkdir    -p  ~/.krb5/
  chmod 0      ~/.krb5
  chmod u=rwx  ~/.krb5
  set file=`echo $KRB5CCNAME | cut -d: -f2`
  if ( -e "$file" ) then
    cp $file ~/.krb5/ticket
  else
    echo " ERROR -- missing kerberos ticket ($KRB5CCNAME)."
    exit 1
  endif
endif

foreach file ($LIST)

  set file=`basename $file`
  #echo $file

  set logFile=`echo $book/$dataset/$file | tr '/' '+'`
  set logFile=/tmp/$logFile
  rm -f $logFile

  set exists=""
  if ( "$condorOutput" != "" ) then
    set exists=`find $catalogDir/condor/$book/$dataset -name $file.out`
  else
    if ( -d "$catalogDir/$book/$dataset" && -f $catalogDir/$book/$dataset/RawFiles.00 ) then
      set exists=`cat $catalogDir/$book/$dataset/RawFiles.?? | grep $file`
    endif
  endif

  #echo EXISTS: $exists

  if ( ".$exists" == "." ) then

    echo "  $script $dataDir/$book/$dataset $file"

    cat > submit_$$.cmd <<EOF
Universe                = vanilla
Requirements            = (MACHINE != "svfnal29.cern.ch") && ( (Arch == "X86_64" || Arch == "INTEL") && (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer) )
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

  endif

end

exit 0


## Requirements            = (Arch == "X86_64" || Arch == "INTEL") && (OpSys == "LINUX") && (Disk >= DiskUsage) && ((Memory * 1024) >= ImageSize) && (HasFileTransfer)
