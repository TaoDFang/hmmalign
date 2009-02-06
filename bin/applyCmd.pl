#!/usr/bin/perl -w

@ARGV>2 || die "Usage: $0 <out-dir> <out-extension> <cmd>\n";

$dir = $ARGV[0];
$ext = $ARGV[1];
$cmd = $ARGV[2];
for ($i=3; $i<@ARGV; $i++){
  $cmd = "$cmd $ARGV[$i]";
}

while(<STDIN>){
  unless(/total \d+/){
    if (/(\S+)\s*$/){
      $file = $1;
      $todo = "$cmd $file $dir/$file\n";
      print $todo;
      system($todo);
    }
  }
}
