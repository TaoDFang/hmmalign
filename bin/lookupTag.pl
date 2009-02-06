#!/usr/bin/perl -w

@ARGV == 1 || die "Usage: $0 <wordTagDictionaryFile>\n";

open(DICT, $ARGV[0]) || die "Could not open $ARGV[0]\n";

%tag=();
while ($pair = <DICT>){
  if ($pair =~ /(\S+)\s+(\S+)/){
    $word=$1;
    $tag=$2;
    $tag{$word} = $tag;
  }
}

while (<STDIN>){
  while (/(\S+)/g){
    $word = $1;
    if (exists($tag{$word})){
      print $tag{$word}." ";
    } else {
      print STDERR "No tag found for $word.\n";
      print $word." ";
    }
  }
  print "\n";
}
