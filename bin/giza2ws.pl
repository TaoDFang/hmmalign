#!/usr/bin/perl

# converts the giza output format to the workshop output format, 
# for AER scoring purposes

$reverse=0;
$nulls=0;
if (@ARGV > 0){
  foreach $arg (@ARGV){
    if ($arg eq "-r"){
      $reverse=1;
    } elsif ($arg eq "-n"){
      $nulls=1;
    } else {
      die "Usage: $0 [-n] [-r]\n";
    }
  }
}

$which=0;
$lineNum=0;
while (<STDIN>){

  $which++;
  if ($which == 1){  # header, do nothing

  } elsif ($which == 2){  # target sentence, do nothing

  } elsif ($which == 3){  # alignment, process
    $lineNum++;

    $src=-1;
    while (/(\S+) (\([^\)]*\))/g){
      $src++;
      $alignments = $2;
      $word = $1;
	
      unless ($alignments =~ /\d/){
	$alignments = "0";
      }
      while ($alignments =~ /(\d+)/g){
	$trg = $1;
	if (($nulls==1) ||
	    (($src>0) && ($trg>0))){
	  unless (($src==0) && ($trg==0)){
	    if ($reverse==1){
	      print "$lineNum $trg $src\n";
	    } else {
	      print "$lineNum $src $trg\n";
	    }
	  }
	}
      }
    }
    $which=0;
  }
}
