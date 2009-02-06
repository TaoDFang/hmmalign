#!/usr/bin/perl -w

@ARGV==3 || die "Usage: <plaindict> <src numbers> <trg numbers>\n";

if ($ARGV[0] eq "-"){
  open(DICT, "cat |") || die "Could not open stdin\n";
} else {
  open(DICT, $ARGV[0]) || die "Could not open $ARGV[0]\n";
}
open(SRC, $ARGV[1]) || die "Could not open $ARGV[1]\n";
open(TRG, $ARGV[2]) || die "Could not open $ARGV[2]\n";


%src=();
$src{"NULL"} = "0";
while (<SRC>){
  if (/^0:(\S+)\s+(\d+)\s+\d+/){
    $src{$1} = $2;
  } 
}

%trg=();
while (<TRG>){
  if (/0:(\S+)\s+(\d+)\s+\d+/){
    $trg{$1} = $2;
  }
}

while (<DICT>){
  if (/^(\S+)\s+(\S+)\s+(\S+)$/){
    unless (exists($src{$1})){ print  "Could not find id for source word $1\n"; }
    unless (exists($trg{$2})){ print "Could not find id for target word $2\n"; }
    if (exists($src{$1}) && exists($trg{$2})){ print "$src{$1} $trg{$2} $3\n"; }
 } else {
    die "Format error: $_\n";
  }
}
