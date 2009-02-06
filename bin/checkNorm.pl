#!/usr/bin/perl -w

%count = ();
while (<>){
  if (/^(.*) [01] (\S+)$/){
    unless (exists($count{$1})){ $count{$1} = 0; }
    $count{$1} += $2;
  }
}

foreach $key (keys(%count)){
  if ($count{$key} > 1.01 || $count{$key} < 0.99){
    print "$key $count{$key}\n";
  }
}
