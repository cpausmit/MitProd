#!/bin/bash
#===================================================================================================
# Download a given filelist from CERN to the Tier-2
#===================================================================================================
FILE="$1"
FLAG="$2"

CERN_LOC="/castor/cern.ch/user/p/paus"
MIT2_LOC="/mnt/hadoop/cms/store/user/paus"

while read line
do

  file=`echo $line | cut -d ' ' -f1`
  echo $file | sed "s#${CERN_LOC}#${MIT2_LOC}#"
  t2File=`echo $file | sed "s#${CERN_LOC}#${MIT2_LOC}#"`

  ssh -x $TICKET_HOLDER@lxplus.cern.ch ./stageFile.py --file=$file
  echo "downloadFile.sh $file $t2File"
  downloadFile.sh $file $t2File

done < $FILE

exit 0
#
#lcg-cp -D srmv2 -b \
#  srm://srm-cms.cern.ch:8443/srm/managerv2?SFN=/castor/cern.ch/user/p/paus/filefi/025/r11b-dmu-pr-v1/06E5C688-98EF-E011-89AD-0025901D61C6.root \
#  srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN=/mnt/hadoop/cms/store/user/paus/filefi/025/r11b-dmu-pr-v1/06E5C688-98EF-E011-89AD-0025901D61C6.root
#
#lcg-cp -D srmv2 -b \
#  srm://srm-cms.cern.ch:8443/srm/managerv2?SFN=/castor/cern.ch/user/p/paus/filefi/025/r11b-dmu-pr-v1/5A934F9E-F0F6-E011-A540-003048D2BA82.root \
#  srm://se01.cmsaf.mit.edu:8443/srm/v2/server?SFN=/mnt/hadoop/cms/store/user/paus/filefi/025/r11b-dmu-pr-v1/5A934F9E-F0F6-E011-A540-003048D2BA82.root
