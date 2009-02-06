#!/usr/bin/perl -w

while (<>){
  if (/^(\d+)\s+(\d+)\s+(\d+)\s*$/){
    print "$1 $3 $2\n";
  } else {
    die "Format error: $_\n";
  }
}
