#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Download a list of files
#---------------------------------------------------------------------------------------------------

# read the arguments
echo ""
echo "downloadFiles.sh  $*"
echo ""
dataDir=$1;      shift
book=$1;         shift
dataset=$1;      shift
target=$1;       shift
condorOutput=$1; shift
first=$1;        shift
last=$1;         shift

# prepare environment
echo " "
echo " Process dataset: $dataset  of book: $book"
echo "   in directory : $dataDir"
echo "   to target    : $target"
echo "   condor output: $condorOutput"
echo "   file range   : $first  -- $last"

mkdir -p $condorOutput/$book/$dataset
script=`which downloadFile.sh`

# make sure the request is good
nFiles=`wc -l $condorOutput/$book/$dataset/fileList.txt | cut -d ' ' -f 1`
if [ $first -gt $nFiles ] ||  [ $last -gt $nFiles ]
then
  echo "Request makes no sense: nFiles=$nFile but first=$first and last=$last"
  exit 0
fi

# see how many we do in this job
nFilesPerJob=$(($last - $first + 1))
fList=`head -$last $condorOutput/$book/$dataset/fileList.txt | tail -$nFilesPerJob | cut -d' ' -f 2`

echo LIST $fList

# spread the jobs out by a bit
sleep $first

# loop through our list now
for file in $fList
do
  file=`basename $file`
  # find the line to this dataset and do further analysis
  line=`grep $file $condorOutput/$book/$dataset/fileList.txt`
  # find potential JSON file
  export size=`echo $line | tr -s ' ' | cut -d ' ' -f 1`
  # now run the download
  echo "$script $dataDir/$book/$dataset/$file $target/$book/$dataset/$file"
  $script $dataDir/$book/$dataset/$file $target/$book/$dataset/$file
done

exit 0
