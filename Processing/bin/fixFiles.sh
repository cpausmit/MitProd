#!/usr/bin/env perl
use strict;
#===================================================================================================
# Assume a list of local files is found to be broken. Go through the list and recover one by one.
#===================================================================================================
my $file = "$ARGV[0]";
my $flag = "$ARGV[1]";
my $line = "";

open(INFO, $file) or die("Could not open file ($file).");
foreach $line (<INFO>)  {   
  print "\nNew file: $line";
  system("fixFile.sh $line $flag")
}
close(INFO);

#FILE="$1"
#FLAG="$2"
#
#while read line
#do
#
#  echo "LINE: $line"
#
#  fullFile=`echo $line | cut -d ' ' -f1`
#  echo fixFile.sh $fullFile $FLAG
#  #fixFile.sh $fullFile $FLAG
#
#done
