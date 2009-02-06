#!/usr/bin/perl -w

$overallMin=1000;
$overallMax=0;
$overallLines=0;
@min = ();
@max = ();

$maxBound = 1000;

if (@ARGV==0){
  open(INPUT, "cat |") || die "Could not open STDIN for reading.\n";
  getStats();
} else {
  foreach $file (@ARGV){
    open (INPUT, $file) || die "Could not $file for reading.\n";
    getStats();
    close(INPUT);
  }
}
@max = sort(@max);
@min = sort(@min);
print "total $overallLines $overallMin $overallMax\n";
print "max = (@max)\n";
print "min = (@min)\n";


sub getStats{
  $min=0;
  $max=0;
  $line = 0;
  while (<INPUT>){
    $len = 0;
    while (/\S+/g){
      $len++;
    }
    if ($len > $max){ $max = $len; }
    if ($line==0 || $min > $len){ $min = $len; }
    $line++;
  }
  if ($maxBound >= $max){
    if ($overallMin > $min){ $overallMin = $min; }
    if ($overallMax < $max){ $overallMax = $max; }
    $overallLines += $line;
    push(@min, $min);
    push(@max, $max);
    print STDOUT "$line $min $max $file\n";
#    print STDOUT "$file\n";
  }
}
