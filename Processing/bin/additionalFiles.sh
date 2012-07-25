#!/bin/bash
#---------------------------------------------------------------------------------------------------
# Find files in new running (open) catalog which are not yet available in the skimmed catalog.
#
#                                                                             Ch.Paus (Jul 29, 2012)
#---------------------------------------------------------------------------------------------------
openCatalog=$1
skimCatalog=$2
book=$3
dataset=$4

if [ -e "$openCatalog/$book/$dataset" ] && [ -e "$skimCatalog/$book/$dataset" ] 
then
  echo ""
  echo " Additional files from"
  echo "  open catalog:"
  echo "   -> $openCatalog/$book/$dataset"
  echo "  with respect to skim catalog:"
  echo "   -> $skimCatalog/$book/$dataset"
else
  echo ""
  echo " ERROR: one of the catalogs does not exist."
  echo "        Open: $openCatalog/$book/$dataset"
  echo "        Skim: $skimCatalog/$book/$dataset"
  echo ""
  exit 0
fi

cat  $openCatalog/$book/$dataset/RawFiles.?? | sort -u > /tmp/open.$$
cat  $skimCatalog/$book/$dataset/RawFiles.?? | sort -u > /tmp/skim.$$
diff /tmp/skim.$$ /tmp/open.$$ | grep \> | cut -d ' ' -f 2-10 \
     > $skimCatalog/$book/$dataset/AddFiles.00
echo ""
echo " Additional files...."
wc   $skimCatalog/$book/$dataset/AddFiles.00

rm /tmp/skim.$$ /tmp/open.$$
exit 0
