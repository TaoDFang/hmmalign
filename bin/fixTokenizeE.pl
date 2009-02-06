#!/usr/bin/perl -w

while (<>){
  s/\'\'/\"/g;
  s/\' ([st])($|\s)/\'$1$2/g;
  print;
}
