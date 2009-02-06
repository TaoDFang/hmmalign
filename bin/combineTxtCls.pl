#!/usr/bin/perl -w

while (<>){
  while (/(\S+)_(\S+)_(\S+)_(\S+)/g){
    print $1."_".$2."_".$3."_".$4."_"."$2-$3 ";
  }
  print "\n";
}
