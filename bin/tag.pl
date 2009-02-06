#!/usr/bin/perl

# Desciption: concatenates each tag in the tag file to 
# the corresponding word in the text file, and writes the
# result to standard out (note: this assumes that the number
# of tags on a line in the tag file matches words on the 
# corresponding line in the text file, or an error will occur.

@ARGV == 2 || @ARGV == 3 || die "Usage: $0 <text file> <tag file> [-n]\n";

$textFile = $ARGV[0];
$tagFile = $ARGV[1];
if ((@ARGV == 3) && ($ARGV[2] eq "-n")){
  $lengthFlag = 1;
} else {
  $lengthFlag = 2;
}


open(TEXT, $textFile) || die "Could not open $textFile\n";
open(TAG, $tagFile) || die "Coudl not $tagFile\n";

$line=0;
while (($text = <TEXT>) && ($tag = <TAG>)){
  $line++;
  $out="";
  $length=0;
  while ($text =~ /(\S+)/g){
    $word=$1;
    $length++;
    if ($tag =~ /(\S+)/g){
      $out = "$out $word"."_"."$1 ";
    } else {
      print STDERR "Warning: length mismatch at line $line\n"; 
    }
  }
  $out =~ s/^\s*(.*)$/$1/;
  if ($lengthFlag == 1){
    $out = "$length $out";
  }
  print "$out\n";
}
