#!/usr/bin/perl

@ARGV==7 || @ARGV==9 || die "Usage: $0 <source dir> <target dir> <source tok dir> <target tok dir> <length-cutoff> <ratio cutoff> <count discards?> [output source file] [output target file]\n";

$sourceDir = $ARGV[0];
$targetDir = $ARGV[1];

$sourceTokDir = $ARGV[2];
$targetTokDir = $ARGV[3];

$maxLen = $ARGV[4];
$maxRatio = $ARGV[5];

$countOnly = $ARGV[6];
if ($countOnly eq "0"){
  @ARGV==9 || die "Error: if <count discards?> == 0, then output file arguments must be given\n";
  $sourceOut = $ARGV[7];
  $targetOut = $ARGV[8];

  open(SOURCEOUT, ">$sourceOut") || die "Could not open file $sourceOut for reading\n";
  open(TARGETOUT, ">$targetOut") || die "Could not open file $targetOut for reading\n";
}

$totalSent = 0;
$tooLong = 0;
$tooDistorted = 0;
$corrupted = 0;


while (<STDIN>) {
  unless(/total/){
    if (/([^\s\\]+)\s*$/) {
      $file = $1;
      $discard = 0;

      open(SOURCE, "$sourceTokDir/$file") || die "Could not open $sourceTokDir/$file for reading\n";
      open(TARGET, "$targetTokDir/$file") || die "Could not open $targetTokDir/$file for reading\n";

      open(SOURCEORIG, "$sourceDir/$file") || die "Could not open $sourceDir/$file for reading\n";
      open(TARGETORIG, "$targetDir/$file") || die "Could not open $targetDir/$file for reading\n";

      $line = 0;
      while ($source = <SOURCE>) {
	$discarded = 0;
	$line++;
	unless ($target = <TARGET>) {
	  die "Unequal lengths: len($sourceTokDir/$file) != len($targetTokDir/$file)\n";
	}
	$totalSent++;

	chomp $source;
	chomp $target;

	$sourceToFile = $source;
	$targetToFile = $target;

	# remove leading and trailing whitespace from both sentences
	$source =~ s/^\s*//;
	$source =~ s/\s*$//;

	$target =~ s/^\s*//;
	$target =~ s/\s*$//;

	@source = split(/ /, $source);
	@target = split(/ /, $target);

	$sourceLen = scalar(@source);
	$targetLen = scalar(@target);

	if ($sourceLen > $maxLen ||
	    $targetLen > $maxLen) {
	  $discard++; 
	  $discarded=1;
	  $tooLong++;
	}
	if ($sourceLen * $maxRatio < $targetLen ||
	    $targetLen * $maxRatio < $sourceLen){
	  $discard++;
	  $discarded=1;
	  $tooDistorted++;
	}
	unless ($source_orig = <SOURCEORIG>){ die "Unequal lengths: len($sourceDir/$file) != len($sourceTokDir/$file)\n"; }
	unless ($target_orig = <TARGETORIG>){ die "Unequal lengths: len($targetDir/$file) != len($targetTokDir/$file)\n"; }
	
	$source =~ s/\s+//g;
	$target =~ s/\s+//g;
	$source_orig =~ s/\s+//g;
	$target_orig =~ s/\s+//g;

	unless ($source eq $source_orig &&
		$target eq $target_orig){
	  $discard++;
	  $discarded=1;
	  $corrupted++;
	}

	if ($discarded==0){
	  print SOURCEOUT "<s id=$file.$line> $sourceToFile </s>\n";
	  print TARGETOUT "<s id=$file.$line> $targetToFile </s>\n";
	}
      }
      if ($countOnly eq "1"){
	print "$discard $file\n";
      }

      if ($target = <TARGET>) {
	die "Unequal lengths: len($sourceTokDir/$file) != len($targetTokDir/$file)\n";
      }
    }
  }
}

$good = $totalSent - $tooLong - $tooDistorted - $corrupted;
print STDERR " $totalSent\n-$tooLong\n-$tooDistorted\n-$corrupted\n====\n$good\n";

