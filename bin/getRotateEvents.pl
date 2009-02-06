#!/usr/bin/perl

@ARGV == 3 || die "Usage: $0 <source file> <target file> <parse file>\n";

open(SOURCE, $ARGV[0]) || die "Could not open $ARGV[0]\n";
open(TARGET, $ARGV[1]) || die "Could not open $ARGV[1]\n";
open(PARSE, $ARGV[2]) || die "Could not open $ARGV[2]\n";

$lastID=0;
$currentLine=0;
@source=();
@target=();
@alignments=();
while (<STDIN>){
  if (/^(\d+)\s+(\d+)\s+(\d+)$/){
    $id = $1;
    $src = $2;
    $trg = $3;

    if ($id > $lastID){
      foreach $src (keys(%alignment)) {
	if (exists($alignment{$parent{$src}})) {
	  @trgParents=split(/ /, $alignment{$parent{$src}});
	  $trgParent=pop(@trgParents);
	  @trg = split(/ /, $alignment{$src});
	  $trg=(@trg);
	  if ($src > $parent{$src}) {
	    $origDirection="right";
	  } else {
	    $origDirection="left";
	  }
	  if ($trg > $trgParent) {
	    $rotate = "right";
	  } else {
	    $rotate = "left";
	  }
	  print "$source[$src] $source[$parent{$src}] $rel{$src} $origDirection $rotate\n";
	}
      }
    }

    until ($currentLine == $id){
      unless (($source = <SOURCE>) &&
	      ($target = <TARGET>) &&
	      ($parse = <PARSE>)){
	die "No more data in input files for id $id\n";
      }
      @source=();
      @target=();
      push(@source, "NULL");
      push(@target, "NULL");

      %parent = ();
      %rel = ();
      while ($parse =~ /(\d+)\s+\S+\s+(\S+)\s+(\d+)\t/g){
	$mod = $1;
	$rel = $2;
	$parent = $3;
	
	$parent{$mod} = $parent;
	$rel{$mod} = $rel;
      }
      while ($source =~ /(\S+)/g){ push(@source, $1); }
      while ($target =~ /(\S+)/g){ push(@target, $1); }
      $currentLine++;
    }

    if (exists($alignment{$src})){
      $alignment{$src} .= " $trg";
    } else {
      $alignment{$src} = $trg;
    }

  } else {
    die "Format error: $_\n";
  }
  $lastID=$id;
}

    foreach $src (keys(%alignment)) {
      if (exists($alignment{$parent{$src}})) {
	@trgParents=split(/ /, $alignment{$parent{$src}});
	$trgParent=pop(@trgParents);
	@trg = split(/ /, $alignment{$src});
	$trg=(@trg);
	if ($src > $parent{$src}) {
	  $origDirection="right";
	} else {
	  $origDirection="left";
	}
	if ($trg > $trgParent) {
	  $rotate = "right";
	} else {
	  $rotate = "left";
	}
	print "$source[$src] $source[$parent{$src}] $rel{$src} $origDirection $rotate\n";
      }
    }

