#!/usr/bin/perl

##########################################################
#
# $Id: numerify.pl,v 1.1.1.1 2003/12/30 03:41:30 alopez Exp $
#
# Author: Adam Lopez
# Purpose: Preprocessing file for alignment inputs
#
##########################################################

$argCount=0;
foreach $arg (@ARGV){
  if ($arg eq "-h"){
    &printHelp;
    exit(0);
  } else {
    $argCount++;
    if ($argCount==1){
      $inputFile=$arg;
      if ($inputFile eq "-"){
	open(IN, "cat |");
      } else {
	open(IN, $inputFile) || die "Error: Could not open $inputFile\n";
      }
    }
    if ($argCount==2){
      $dictFile=$arg;
      open(DICT, ">$dictFile") || die "Error: Could not open $dictFile for writing\n";
    }
  }
}

$argCount == 2 || die "Error: wrong number of arguments. Use -h for help.\n";

while(<IN>){
  s/^\s+//;
  s/\s\s+/ /g;
  s/\s+$//;
  @words = split(/ /, $_);
  foreach $word (@words){
    @feature = split(/_/, $word);
    for ($i=0; $i<@feature; $i++){
      if ($i >= @featureCount){
	push(@featureCount, 0);
      }
      $key="$i:$feature[$i]";
      unless (exists($map{$key})){
	$featureCount[$i]++;
	$map{$key} = $featureCount[$i];
	$count{$key} = 0;
      }
      $count{$key}++;
      if ($i>0){
	print "_";
      }
      print "$map{$key}";

    }
    print " ";
  }
  print "\n";
}

$numFeatures = scalar(@featureCount);
print DICT "$numFeatures ";
foreach $count (@featureCount){
  print DICT "$count ";
}
print DICT "\n";
foreach $key (sort(keys(%map))){
  print DICT "$key $map{$key} $count{$key}\n";
}
close(DICT);

sub printHelp
{
    print << "Help";

usage: $0 <infile> <dictfile>

$0 takes an input alignment file and converts all of the 
tokens in the file to unique integer ids.  The token-id 
mapping is written to the file named in the argument
<dictfile>.  The argument <infile> should be the
input file; use "-" to take input from standard in.

Flags:
     -h
       Prints this message.
 
Help
}


##########################################################
#
# $log$
#
##########################################################

