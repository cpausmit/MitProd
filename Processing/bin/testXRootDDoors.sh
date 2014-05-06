#!/bin/bash
#===================================================================================================
# Test the main xrootd redirector and each of the define xrootd doors.
#
#                                                                          C.Paus (V0: Apr 22, 2014)
#===================================================================================================
XROOTD_MAIN="xrootd"
DOMAIN="cmsaf.mit.edu"
DOORS='1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20'

testFile="/store/user/paus/test.root"

cmd="root -l -b -q root://${XROOTD_MAIN}.${DOMAIN}/${testFile}"
echo -n " Test main server: $cmd -- "
$cmd
rc="$?"
echo " (status: $rc)"
if [ "$rc" != "0" ]
then
  echo " ERROR - failure to open test file"
fi

for door in $DOORS
do
  cmd="root -l -b -q root://${XROOTD_MAIN}${door}.${DOMAIN}/${testFile}"
  echo -n " Test door $door: $cmd -- "
  $cmd
  rc="$?"
  echo " (status: $rc)"
  if [ "$rc" != "0" ]
  then
    echo " ERROR - failure to open test file"
  fi
  echo " "
done
