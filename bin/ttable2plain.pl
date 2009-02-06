#!/usr/bin/perl -w

@ARGV==3 || die "Usage: ttable2plain.pl <ttable> <src vocab file> <trg vocab file>\n";

open(TTABLE, $ARGV[0]) || die "Could not open $ARGV[0]\n";
open(SRCVOC, $ARGV[1]) || die "Could not open $ARGV[1]\n";
open(TRGVOC, $ARGV[2]) || die "Could not open $ARGV[2]\n";

%src = ();
$src{"0"}= "NULL";
while (<SRCVOC>){
  if (/(\d+)\s+(\S+)\s+\d+/){
    $src{$1} = $2;
  } else {
    die "Format error: $_\n";
  }
}

%trg = ();
while (<TRGVOC>){
  if (/(\d+)\s+(\S+)\s+\d+/){
    $trg{$1} = $2;
  } else {
    die "Format error: $_\n";
  }
}

while(<TTABLE>){
  if (/(\d+)\s+(\d+)\s+(\S+)/){
    unless (exists($src{$1})){ die "Error: could not match source id $1\n"; }
    unless (exists($trg{$2})){ die "Error: could not match target id $2\n"; }
    print "$src{$1} $trg{$2} $3\n";
  }
}
