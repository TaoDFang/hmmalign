#!/usr/bin/perl

@ARGV==2 || die "Usage: getRotateStats.pl <input file> <alignment file>\n";

open(INPUT, $ARGV[0]) || die "Could not open $ARGV[0]\n";
open(ALIGN, $ARGV[1]) || die "Could not open $ARGV[1]\n";

# discard first two lines of input.
$input = <INPUT>; # e stats
$input = <INPUT>; # f stats

$right = "1";
$left = "0";

@queue = ();
$sentNum = 0;
while ($input = <INPUT>){
  $sentNum++;
  if ($input = <INPUT>){
    @e=(); push(@e, "TOP");
    while ($input =~ /(\S+)/g){
      push(@e, $1);
    }

    if ($input = <INPUT>){
      @f=(); push(@f, "TOP");
      while ($input =~ /(\S+)/g){
	push(@f, $1);
      }

      %parent = ();
      %children = ();
      if ($input = <INPUT>){
	while ($input =~ /(\d+) (\d+)/g){
	  $parent{$1} = $2;
	  if (exists($children{$2})){
	    $children{$2} .= " $1";
	  } else {
	    $children{$2} = $1;
	  }
	}
      }

      while ($align = <ALIGN>){
	if ($align =~ /(\d+)\s+(\d+)\s+(\d+)/){
	  push(@queue, $align);
	  if ($1 ne $sentNum){
	    last;
	  }
	} else {
	  die "Format error in $ARGV[1]: $align";
	}
      }
      @auxqueue = ();

      %alignment=();
      foreach $line (@queue){
	if (($line =~ /(\d+)\s+(\d+)\s+(\d+)/) &&
	    ($1 eq $sentNum)){
	  if (exists($alignment{$2})){
	    $alignment{$2} .= " $3";
	  } else {
	    $alignment{$2} = $3;
	  }
	} else {
	  push(@auxqueue, $line);
	}
      }
      @queue = @auxqueue;


      for ($i=1; $i<@e; $i++){
	$child = $i;
	$parent = $parent{$i};
	if ($child > $parent){
	  $origDir = $right;
	} else {
	  $origDir = $left;
	}
	$rightCount=0;
	$leftCount=0;

	if (($parent > 0) && !($alignment{$parent} eq "0")){
	  foreach $pword (split(/ /, $alignment{$parent})){
	    if ($alignment{$child} eq "0"){
	      if (exists($children{$child})){
		foreach $grandchild (split(/ /, $children{$child})){
		  if (!($alignment{$grandchild} eq "0")){
		    foreach $gword (split(/ /, $alignment{$grandchild})){
		      if ($gword > $pword){
			$rightCount++;
		      } else {
			$leftCount++;
		      }
		    }
		  }
		}
	      } else {
		$rightCount += 0.5;
		$leftCount += 0.5;
	      }
	    } else {
	      foreach $cword (split(/ /, $alignment{$child})){
		if ($cword > $pword){
		  $rightCount++;
		} else {
		  $leftCount++;
		}
	      }
	    }
	  }
	}
	$totalCount = $rightCount+$leftCount;
	if ($rightCount > 0){
	  $rightCount = $rightCount/$totalCount;
	  if ($e[$child] =~ /^(\d+)_\d+_\d+_(\d+)/){
	    $cword = $1; $rel = $2;
	    if ($e[$parent] =~ /^(\d+)/){
#	      $key = "$cword $rel $1 $origDir $right";
	      $key = "$rel $origDir $right";
	      if (exists($count{$key})){
		$count{$key}+=$rightCount;
	      } else {
		$count{$key} = $rightCount;
	      }
	    }
	  }
	} 
	if ($leftCount > 0){
	  $leftCount = $leftCount/$totalCount;
	  if ($e[$child] =~ /^(\d+)_\d+_\d+_(\d+)/){
	    $cword = $1; $rel = $2;
	    if ($e[$parent] =~ /^(\d+)/){
#	      $key = "$cword $rel $1 $origDir $left";
	      $key = "$rel $origDir $left";
	      if (exists($count{$key})){
		$count{$key}+=$leftCount;
	      } else {
		$count{$key} = $leftCount;
	      }
	    }
	  }
	}
      }
    }
  }
}

format OVERONE =
@.#####
$count
.

format UNDERONE =
@.######
$count
.

format OVERTEN = 
@.####
$count
.

format OVERHUN = 
@.###
$count
.

format OVERTHOU = 
@.##
$count
.

foreach $key (sort(keys(%count))){
  $count = $count{$key};
  print "$key ";
  print "$count\n";
#  if ($count =~ /^0\.\d{6,}/){
#    $~ = UNDERONE;
#    write();
#  } elsif ($count =~ /^\d\.\d{5,}/){
#    $~ = OVERONE;
#    write();
#  } elsif ($count =~ /^\d\d\.\d{4,}/){
#    $~ = OVERTEN;
#    write();
#  } elsif ($count =~ /^\d\d\d\.\d{3,}/){
#    $~ = OVERHUN;
#    write();
#  } elsif ($count =~ /^\d\d\d\d\.\d{2,}/){
#    $~ = OVERTHOU;
#    write();
#  } else {
#    print "$count\n";
#  }
}
