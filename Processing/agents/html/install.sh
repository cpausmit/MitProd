#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Install the web environment (should be done once at installation phase only). Make sure the
# environment is setup correctly
# 
#
# Author: C.Paus                                                                     (May 14, 2015)
#---------------------------------------------------------------------------------------------------

# images
cp $MIT_PROD_DIR/agents/html/agent*jpg $MIT_PROD_AGENTS_WWW

# index files to log area
cp $MIT_PROD_DIR/agents/html/index.html $MIT_PROD_AGENTS_LOG

cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/reviewd/' | sed 's/XX-AKA-XX/Smith/' \
   > $MIT_PROD_AGENTS_LOG/reviewd/index.php
cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/catalogd/' | sed 's/XX-AKA-XX/Johnson/' \
   > $MIT_PROD_AGENTS_LOG/catalogd/index.php
cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/cleanupd/' | sed 's/XX-AKA-XX/Williams/' \
   > $MIT_PROD_AGENTS_LOG/cleanupd/index.php

# update web pages from log area
echo " Sync webpages - no deletions"
mkdir -p $MIT_PROD_AGENTS_WWW 
rsync -Cavz $MIT_PROD_AGENTS_LOG $MIT_PROD_AGENTS_WWW/../


exit 0
