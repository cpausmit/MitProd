#!/bin/bash

#!/bin/bash
#this script is run on condor; all paths must be absolute, preferably on /scratch or

indir=/mnt/hscratch/snarayan/mc/ttjets
scramdir=/home/snarayan/cms/cmssw/041/CMSSW_7_4_4
workdir=/condor/execute/dir_$PPID/

nproc=$1
fileListPath=${scramdir}"/src/MitProd/scripts/ttjets.cfg"
echo $fileListPath
cd ${scramdir}/src
eval `scramv1 runtime -sh`
cd $workdir
pwd
cp ${scramdir}/src/MitProd/Configuration/python/BambuProdAodsim_localBatch.py .

cmsRun BambuProdAodsim_localBatch.py fileListPath=${fileListPath} nFile=${nproc}

status=`echo $?`
echo "Status - $status"

mv bambu*root ${indir}/bambu_${val}.root
exit $status
