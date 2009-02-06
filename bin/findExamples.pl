#!/usr/bin/perl -w

@ARGV==1 || die "Usage: $0 <char>\n";

$char = $ARGV[0];

while (<STDIN>){
  while (/(^|\S+)(\s*)($char)(\s*)(\S+|$)/g){
    $wordBefore = $1;
    $spaceBefore = $2;
    $searchExpr = $3;
    $spaceAfter = $4;
    $wordAfter = $5;
    unless ($wordAfter =~ /\S/){ $spaceAfter = ""; $wordAfter = ""; }
    unless ($wordBefore =~ /\S/){ $wordBefore = ""; $spaceBefore = ""; }
    print $wordBefore . $spaceBefore . $searchExpr . $spaceAfter . $wordAfter . "\n";;
  }
}
