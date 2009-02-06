#!/usr/bin/perl

@ARGV == 3 || die "Usage: makeOneBigCorpus.pl <filepairs> <source> <target>\n";

if ($ARGV[0] eq "-"){
  open(FILEPAIRS, "cat |") || die "Could not open stdin for reading.\n";
} else {
  open(FILEPAIRS, $ARGV[0]) || die "Could not open $ARGV[0] for reading.\n";
}
open (SOURCEOUT, ">$ARGV[1]") || die "Could not open $ARGV[1] for writing.\n";
open (TARGETOUT, ">$ARGV[2]") || die "Could not open $ARGV[2] for writing.\n";

while ($pair = <FILEPAIRS>){
  if ($pair =~ /^(\S+)\s+(\S+)\s*$/){
    open(SOURCE, $1) || die "Could not open $1\n";
    open(TARGET, $2) || die "Could not open $2\n";
    while (($source = <SOURCE>) &&
	   ($target = <TARGET>)){
      $sourceLen = 0; while ($source =~ /\S+/g){ $sourceLen++; }
      $targetLen = 0; while ($target =~ /\S+/g){ $targetLen++; }

      if (
	  $sourceLen > 0 &&
	  $targetLen > 0 &&
	  $sourceLen*1.5 >= $targetLen &&
	  $targetLen*1.5 >= $sourceLen
	 ){
	print SOURCEOUT $source;
	print TARGETOUT $target;
      }
    }
    close(SOURCE);
    close(TARGET);
  } else {
    die "File format error: $pair\n";
  }
}
