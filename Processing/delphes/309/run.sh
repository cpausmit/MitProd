#!/bin/sh
#===================================================================================================
#
# Script to run a delphes job file by file.
#
#===================================================================================================

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX id; env; pwd"
id; env; pwd

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ls -lhrt"
ls -lhrt

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ls -lhrt *"
ls -lhrt *

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX find ./"
find ./

# Here is where our stuff happens

export      JOBID=$1
export    DATADIR=$2
export     CONFIG=`echo *_crab_0_*.py | sed 's#_crab_0_[0-9].*\.py#.tcl#'`
cp *_crab_0_*.py $CONFIG
export     OUTPUT=`echo *lfns_crab_0_*  | cut -d '.' -f 1`_000.root
export InputFiles=`head -$JOBID *.lfns_* | tail -1 | tr -s ' ' | cut -d ' ' -f2`

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "Produce the input files: $InputFiles"
echo "  input files: $InputFiles"

# convert LFN into the local file name at the site
INPUT=`edmFileUtil -d $InputFiles`

echo ""
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "Fake configuration file (py):  fake.py"
echo "Real configuration file (tcl): $CONFIG"

export LD_LIBRARY_PATH=".:${LD_LIBRARY_PATH}"
echo   "ldd ./DelphesCMSFWLite"
ldd         ./DelphesCMSFWLite

# when running at MIT
ln -s /mnt/hadoop/cms/store/user/paus/minbias/minbias_14tev.pileup ./MinBias.pileup

# when running somewhere else
#which wget
#wget http://t3serv001.mit.edu/~klute/delphes/MinBias100K_14TeV.pileup \
#     -O MinBias.pileup

echo ""
echo "Start running at `date -u`"
echo ""
start_exe_time=`date +%s`
CPU_INFOS=-1

date >  xxx
echo "./DelphesCMSFWLite $CONFIG $OUTPUT $INPUT"
/usr/bin/time       \
  -f "%U %S %P"     \
  -o cpu_timing.txt \
      ./DelphesCMSFWLite $CONFIG $OUTPUT $INPUT
date >> xxx
cat cpu_timing.txt >> xxx

## /usr/bin/time       \
##   -f "%U %S %P"     \
##   -o cpu_timing.txt \
##   cmsRun            \
##   -j ${RUNTIME_AREA}/crab_fjr_${JOBID}.xml \
##   -p cmssw_ex.py

cmsrun_exit_status=$?

CPU_INFOS=`tail -n 1 cpu_timing.txt`
stop_exe_time=`date +%s`

echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "Running ended at `date -u`"
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo ""

echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "Present working directory is `pwd`"
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo ""

echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo "Directory content is `ls -l`"
echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
echo ""

exit 0
