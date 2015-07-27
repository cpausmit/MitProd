#!/bin/bash

outdir=/afs/cern.ch/user/s/snarayan/work/mc/ttjets
scramdir=/afs/cern.ch/user/s/snarayan/work/private/CMSSW_7_4_4/
workdir=$PWD

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

mv bambu*root ${outdir}/bambu_${val}.root
exit $status
