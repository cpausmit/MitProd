#!/bin/bash

# Install the web environment (should be done once at installation phase only)

# images
cp $MIT_PROD_DIR/agents/html/agent*jpg $HOME/public_html/MitProd/agents

# index files
cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/reviewd/' | sed 's/XX-AKA-XX/Smith/' \
   > $MIT_PROD_AGENTS_LOG/reviewd/index.php
cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/catalogd/' | sed 's/XX-AKA-XX/Johnson/' \
   > $MIT_PROD_AGENTS_LOG/catalogd/index.php
cat $MIT_PROD_DIR/agents/html/index.php-Template \
   | sed 's/XX-NAME-XX/cleanupd/' | sed 's/XX-AKA-XX/Williams/' \
   > $MIT_PROD_AGENTS_LOG/cleanupd/index.php

exit 0
