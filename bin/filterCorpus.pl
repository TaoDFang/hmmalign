#!/usr/bin/perl

@ARGV==4 || die "Usage: $0 <max length> <max ratio> <E file> <F file>\n";

$maxLen = $ARGV[0];
$maxRatio = $ARGV[1];

$eFile = $ARGV[2];
$fFile = $ARGV[3];

$eFileOut = $eFile.".filtered.$maxLen.$maxRatio";
$fFileOut = $fFile.".filtered.$maxLen.$maxRatio";

open(EFILE, $eFile) || die "Could not open $eFile for reading\n";
open(FFILE, $fFile) || die "Could not open $fFile for reading\n";

open(EFILEOUT, ">$eFileOut") || die "Could not open $eFileOut for writing\n";
open(FFILEOUT, ">$fFileOut") || die "Could not open $fFileOut for writing\n";

while (($e = <EFILE>) &&
       ($f = <FFILE>)){

  @e = split(/\s+/, $e);
  @f = split(/\s+/, $f);

  $eLen = scalar(@e);
  $fLen = scalar(@f);
  
  unless ($eLen > $maxLen ||
	  $fLen > $maxLen ||
	  $eLen * $maxRatio < $fLen ||
	  $fLen * $maxRatio < $eLen){
    print EFILEOUT $e;
    print FFILEOUT $f;
  }
}
