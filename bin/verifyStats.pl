#!/usr/bin/perl -w

$current = -1;
while (<>){
  if (/^(\d+)\s+([\d\.]+)\s*$/){
    print $_;
  }
}
