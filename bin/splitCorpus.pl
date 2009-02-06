#!/usr/bin/perl

@ARGV==2 || @ARGV==3 || die "Usage: $0 <corpus> <increment size> [-x]\n";

(-e $ARGV[0]) || die "Could not find file $ARGV[0]\n";

open(LENGTH, "wc $ARGV[0]|");
$increment=$ARGV[1];
$increment =~ /^\d+$/ || die "Second argument must be positive integer, found $increment\n";

if (@ARGV==3 && $ARGV[2] eq "-x"){
  $execute = 1;
} else {
  $execute = 0;
}

while (<LENGTH>){
  print STDERR;

  if  (/^\s*(\d+)/){
    $length = $1;

    print "corpus length = $length\n";

    $needDigits=0;
    while ($length =~ /\d/g){ $needDigits++; }

    for ($i=$increment; $i <= $length; $i+=$increment){
      $firstLine = $i-$increment+1;
      $lastLine = $i;

      $digits = 0;
      while ($firstLine =~ /\d/g){ $digits++; }
      for ($j=$digits; $j < $needDigits; $j++){
	$firstLine = "0".$firstLine;
      }

      $digits = 0;
      while ($lastLine =~ /\d/g){ $digits++; }
      for ($j=$digits; $j < $needDigits; $j++){
	$lastLine = "0".$lastLine;
      }

      $cmd = "head -$i $ARGV[0] | tail -$increment > $ARGV[0].$firstLine-$lastLine\n";
      print STDERR $cmd;
      if ($execute == 1) { system($cmd); }
    }
    $i-=$increment;
    if ($i < $length){
      $firstLine = $i+1;
      $lastLine = $length;

      $tail = $length - $i;

      $digits = 0;
      while ($firstLine =~ /\d/g){ $digits++; }
      for ($j=$digits; $j < $needDigits; $j++){
	$firstLine = "0".$firstLine;
      }

      $digits = 0;
      while ($lastLine =~ /\d/g){ $digits++; }
      for ($j=$digits; $j < $needDigits; $j++){
	$lastLine = "0".$lastLine;
      }

      $cmd = "tail -$tail $ARGV[0] > $ARGV[0].$firstLine-$lastLine\n";
      print STDERR $cmd;
      if ($execute == 1) { system($cmd); }
    }
  }
}
