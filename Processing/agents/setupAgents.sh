#---------------------------------------------------------------------------------------------------
#
# Here the main parameters for request review are defined. This is probably not the way we want to
# do it on the long run but let's start this way.
#
#---------------------------------------------------------------------------------------------------
# Environment: CMSSW, CRAB etc.

# find latest MIT version
export MIT_VERS=`ls -1 ~cmsprod/cms/cmssw| grep ^[0-9]| tail -1`
# find latest CMSSW version
export LATEST_CMSSW=`ls -rt ~cmsprod/cms/cmssw/$MIT_VERS| grep ^CMSSW_[0-9] | tail -1`

export MIT_TAG=Mit_$MIT_VERS
export TICKET_HOLDER="paus"
export TIER2_USER="paus"
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd     ~cmsprod/cms/cmssw/${MIT_VERS}/${LATEST_CMSSW}/src
eval   `scram runtime -sh`
cd     -
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh

source /home/cmsprod/Tools/FiBS/setup.sh
source /home/cmsprod/Tools/T2Tools/setup.sh

# general info

export MIT_PROD_AGENTS_BASE="/usr/local/MitProd/agents"
export MIT_PROD_AGENTS_WORK="/home/cmsprod/cms/jobs"
export MIT_PROD_AGENTS_LOG="/local/cmsprod/MitProd/agents"
export MIT_PROD_AGENTS_WWW="/home/cmsprod/public_html/MitProd/agents"

# review process

export MIT_PROD_REVIEW_CYCLE_HOURS=1
## export MIT_PROD_REVIEW_PYS="data mc"
export MIT_PROD_REVIEW_PYS="data mc"

# catalog process

export MIT_PROD_CATALOG_CYCLE_SECONDS=300

# cleanup process

export MIT_PROD_CLEANUP_CYCLE_SECONDS=3600

# upload process

export MIT_PROD_UPLOAD_CYCLE_SECONDS=300
export MIT_PROD_UPLOAD_THRESHOLD=10
export MIT_PROD_UPLOAD_NTRANSFERS=5
export MIT_PROD_UPLOAD_BOOK="filefi/$MIT_VERS"
