#---------------------------------------------------------------------------------------------------
#
# Here the main parameters for request review are defined. This is probably not the way we want to
# do it on the long run but let's start this way.
#
#---------------------------------------------------------------------------------------------------
# Environment: CMSSW, CRAB etc.
export MIT_VERS=042
export MIT_TAG=Mit_042
export TICKET_HOLDER="paus"
export TIER2_USER="paus"
export CRAB_VERS="2_11_1_patch1"
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd ~cmsprod/cms/cmssw/042/CMSSW_7_4_6/src
eval `scram runtime -sh`
cd -
source $CMSSW_BASE/src/MitProd/Processing/bin/processing.sh
source /cvmfs/cms.cern.ch/crab/CRAB_${CRAB_VERS}/crab.sh

# general info

export MIT_PROD_AGENTS_BASE="/usr/local/MitProd/agents"
export MIT_PROD_AGENTS_WORK="/home/cmsprod/cms/jobs"
export MIT_PROD_AGENTS_LOG="/local/cmsprod/MitProd/agents"
export MIT_PROD_AGENTS_WWW="/home/cmsprod/public_html/MitProd/agents"

# review process

export MIT_PROD_REVIEW_CYCLE_HOURS=1
export MIT_PROD_REVIEW_PYS="data mc mc-notrig"

# catalog process

export MIT_PROD_CATALOG_CYCLE_SECONDS=300

# catalog process

export MIT_PROD_CLEANUP_CYCLE_SECONDS=3600
