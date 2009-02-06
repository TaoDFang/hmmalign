#!/usr/bin/perl 
# converts the alignment format from Philipp Koehn's
# Pharaoh script to the alignment workshop format
# for easy evaluation

$linenum = 0;
while (<>){
  $linenum++;
  
  s/^\s*(\S.*\S)\s*$/$1/;
  s/\s+/ /g;
  @line = split(/ /);
  
  foreach $alignment (@line){
    $alignment =~ /^(\d+)-(\d+)$/ || 
      die "Format error, line $linenum, alignment $alignment\n";
    $sourceIndex = $1+1;
    $targetIndex = $2+1;
    print "$linenum $sourceIndex $targetIndex\n";

  }
}
