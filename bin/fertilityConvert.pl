#!/usr/bin/perl -w

@ARGV==3 || die "Usage: <fertility table> <GIZA++ dictionary> <src dictionary>\n";

if ($ARGV[0] eq "-"){
  open(FERT, "cat |") || die "Could not open stdin\n";
} else {
  open(FERT, $ARGV[0]) || die "Could not open $ARGV[0]\n";
}
open(GIZA, $ARGV[1]) || die "Could not open $ARGV[1]\n";
open(NUM, $ARGV[2]) || die "Could not open $ARGV[2]\n";


%giza=();
while (<GIZA>){
  if (/^(\d+)\s+(\S+)\s+\d+/){
    $giza{$1}=$2;
  } else {
    die "Format error: $_\n";
  }
}
close(GIZA);

%num=();
while (<NUM>){
  if (/0:(\S+)\s+(\d+)\s+\d+/){
    $num{$1} = $2;
  }
}
close(NUM);

while (<FERT>){
  if (/^(\d+)\s+(\S+)\s+\S+/){
    unless (exists($giza{$1})){ die "Could not find word for GIZA++ id $1\n"; }
    unless (exists($num{$giza{$1}})){ die "Could not find id for target word $giza{$1}\n"; }
    print "$num{$giza{$1}} $2\n";
 } else {
    die "Format error: $_\n";
  }
}
