#!/usr/bin/perl

@ARGV==2 || die "Usage: $0 <src file> <trg file>\n";

open(SOURCE, $ARGV[0]) || die "Could not open $ARGV[0]\n";
open(TARGET, $ARGV[1]) || die "Could not open $ARGV[1]\n";

$n=0;
$maxlen = 0;
$maxRatio = 0;
while ($src = <SOURCE>){
  $n++;
  if ($trg = <TARGET>){
    @source = split(/ /, $src);
    @target = split(/ /, $trg);
    $srclen = @source - 1;
    $trglen = @target - 1;

    $ratio = $srclen / $trglen;
    if ($ratio < 1){ $ratio = $trglen / $srclen; }

    if ($srclen > $maxlen){ $maxlen = $srclen; }
    if ($trglen > $maxlen){ $maxlen = $trglen; }
    if ($ratio > $maxRatio){ $maxRatio = $ratio; }

  } else {
    die "Error: $ARGV[0] longer than $ARGV[1] at line $n\n";
  }
}

if ($trg = <TARGET>){
  $n++;
  die "Error: $ARGV[1] longer than $ARGV[0] at line $n\n";
}

print "MAXIMUM SENTENCE LENGTH = $maxlen\nMAX RATIO = $maxRatio\n";;
