#!/usr/bin/perl -w

while(<>){
  $len = 0;
  while (/\S+/g){
    $len++;
  }
  print "$len\n";
}
