#!/usr/bin/perl -w

@ARGV>=1 || die "Usage: $0 <threshhold> [-r]\n";

$refined = 0;

$whichParam = 0;
foreach $arg (@ARGV){
  if ($arg eq "-r"){
    $refined = 1;
  } else {
    $whichParam++;
    if ($whichParam==1){
      $threshhold = $arg;
      unless ($threshhold =~ /^\d+$/){
	die "Error: argument 1 (theshhold) must be a positive integer.\n";
      }
    }
  }
}
if ($whichParam != 1){
  die "Usage: $0 <threshhold> [-r]\n";
}

while ($link=<STDIN>){
  unless ($link=~/(\d+)\s+(\d+\s+\d+)/){
    die "Format error: $link\n"
  }
  $sentId=$1;
  $link=$2;

  while ($sentId !~ /\d{10}/){
    $sentId = "0$sentId";
  }

  if (exists($links{$sentId})){
    $links{$sentId} .= ":$link";
  } else {
    $links{$sentId} = $link;
  }
}

foreach $sentId (sort(keys(%links))){
  @links = split(/:/,$links{$sentId});
  %linkCount=();
  foreach $link (@links){
    while ($link !~ /^\d{4}/){
      $link = "0$link";
    }
    if (exists($linkCount{$link})){
      $linkCount{$link}++;
    } else {
      $linkCount{$link}=1;
    }
  }
  while ($sentId=~ /^0(\d+)$/){ $sentId=$1; }

  %printed=();
  $change=1;
  while ($change == 1){
    $change = 0;
    foreach $link (sort(keys(%linkCount))) {
      if ($linkCount{$link} >= $threshhold) {
	while ($link =~ /^0(\d+\s+\d+)$/) {
	  $link=$1;
	}
	unless (exists($printed{$link})){
	  print "$sentId $link\n";
	  $printed{$link}=1;
	}
      } else {
	if ($refined == 1){
	  if ($link =~ /^0+(\d+)\s+(\d+)$/) {
	    $src=$1;
	    $trg=$2;

	    $uplink= ($src+1)." $trg";     while ($uplink !~ /^\d{4}/){ $uplink = "0$uplink"; }
	    $downlink= ($src-1)." $trg";   while ($downlink !~ /^\d{4}/){ $downlink = "0$downlink"; }
	    $leftlink= "$src ".($trg-1);   while ($leftlink !~ /^\d{4}/){ $leftlink = "0$leftlink"; }
	    $rightlink= "$src ".($trg+1);  while ($rightlink !~ /^\d{4}/){ $rightlink = "0$rightlink"; }

	    if ((exists($linkCount{$uplink}) && ($linkCount{$uplink} >= $threshhold)) ||
		(exists($linkCount{$downlink}) && ($linkCount{$downlink} >= $threshhold))) {
	      $updown = 1;
	    } else {
	      $updown = 0;
	    }

	    if ((exists($linkCount{$leftlink}) && ($linkCount{$leftlink} >= $threshhold)) ||
		(exists($linkCount{$rightlink}) && ($linkCount{$rightlink} >= $threshhold))) {
	      $leftright = 1;
	    } else {
	      $leftright = 0;
	    }

	    if ((($updown == 1) && ($leftright == 0)) ||
		(($updown == 0) && ($leftright == 1))) {
	      $linkCount{$link} = $threshhold;
	      $change=1;
	    }
	  }
	}
      }
    }
  }
}
