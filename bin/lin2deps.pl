#!/usr/bin/perl -w

@ARGV==2 || @ARGV == 3 || die "Usage: $0 <text file> <parse file> [-i]\n";

if ((@ARGV==3) && ($ARGV[2] eq "-i")){
  $for_input=1;
} else {
  $for_input=0;
}

open(TEXT, $ARGV[0]) || die "Could not open $ARGV[0]\n";
open(PARSE, $ARGV[1]) || die "Could not open $ARGV[1]\n"; 

$lineNum=0;
while ($text = <TEXT>){
  $lineNum++;
  @words = ("TOP");
  %parent=();
  %mapping=();
  %revMapping=();
  %relation=();
  %tag=();
  $mapping{"0"} = "0";
  $revMapping{"0"} = "0";
  $offset=0;
  while ($text =~ /(\S+)/g){
    push(@words, $1);
  }
  while ($parse = <PARSE>){
    if ( $parse =~/^\)$/ ){ 
      last;
    }
    if ($parse =~ /(\S+)\t\(([^\t]+)\t([^\t]+)\t([^\t]+)/){
#      print "$1 $2 $3\n";
      $position = $1;
      $word = $2;
      $tag = $3;
      $parent = $4;

      $tag =~ s/.*\s(\S+)$/$1/;

      if ($parse =~ /\S+\t\([^\t]+\t[^\t]+\t[^\t]+\t([^\s\)]+)/){
	$rel=$1;
      } else {
	$rel="unknown";
      }

      $word =~ s/\"\(\"/\(/;
      $word =~ s/\"\)\"/\)/;

      unless($position =~ /[a-zA-Z]/){
	if (($word =~ /^\"(.*)\"$/) &&
	    (($1 eq $words[$position+$offset]) ||
	     ($1."@" eq $words[$position+$offset]))){
	  $word = $1;
	}
	if ($word eq "\"\\\"\""){ $word = "\""; }
	if ($word eq "\"\\\\\""){ $word = "\\"; }
	
#	print STDERR "$lineNum: $position $offset $word ".$words[$position+$offset]."\n";
	until (($position==-$offset) ||
	       (($position+$offset < @words) && ($words[$position+$offset] =~ /\Q$word\E/))){
	  $offset--;
#	  print STDERR "$lineNum: $position $offset $word ".$words[$position+$offset]."\n";
	}

	if ($position == -$offset){
	  print STDERR "Word mismatch: $word vs. ".$words[$position]." at line $lineNum, position $position\n";
	} else {
	  $mapping{$position} = $position+$offset;
	  if (exists($revMapping{$position+$offset})){
	    $revMapping{$position+$offset} .= "$position ";
	  } else {
	    $revMapping{$position+$offset} = " $position ";
	  }
	}
      }
      if ($parent eq "*"){ $parent = "0"; }
      $parent{$position} = $parent;
      $relation{$position} = $rel;
      $tag{$position} = $tag;
    }
  }

  foreach $word (keys(%parent)){
    $index = $word;
    while (exists($parent{$index}) &&
	   $parent{$index} =~ /[a-zA-Z]/){
      $index = $parent{$index};
    }
    if (exists($parent{$index})){
      $parent{$word} = $parent{$index};
    } else {
      print STDERR "No parent found for word at index $word on line $lineNum\n";
    }
  }

  %outParent=();
  %outRelation=();
  %outTag=();
  for($i=1; $i<@words; $i++){
    $rel = "unknown";
    $tag = "unknown";
    if (exists($revMapping{$i})){
      if ($revMapping{$i} =~ /^ (\d+) $/){
	if (exists($parent{$1})){
	  $parent = $mapping{$parent{$1}};
	  $rel = $relation{$1};
	  $tag = $tag{$1};
	} else {
	  print STDERR "Could not find parent of $lineNum.$i ($1) revMapping={".$revMapping{$i}."}\n";
	}
      } else {
#	print "Multi-mapped word $lineNum.$i: $revMapping{$i}\t";
	@elements=();
	%parentElements=();
	while ($revMapping{$i} =~ /(\d+)/g){
	  push(@elements, $1);
	}
	foreach $element (@elements){
	  unless ($revMapping{$i} =~ / $parent{$element} /){
	    $parentElements{$parent{$element}}=1;
	    $rel = $relation{$element};
	    $tag = $tag{$element};
	  }
	}
	@possibleParents = keys(%parentElements);
	if (@possibleParents == 1){
	  $parent = $mapping{$possibleParents[0]};
	} else {
	  @possibleParents = keys(%parentElements);
#	  print STDERR "$lineNum.$i: For word $words[$i], possible parents in parse are @possibleParents (out of parents of @elements)\n";
	}
      }
      $outParent{$i}=$parent;
      $outRelation{$i}=$rel;
      $outTag{$i}=$tag;
    }
  }
  for ($i=1; $i<@words; $i++){
    unless (exists($outParent{$i})){

      # find connected word to the left
      for ($left=$i-1; $left>0 && !(exists($outParent{$left})); $left--){}
      if ($left==0){ 
	$outParent{$i} = 0; 
	#print STDERR "Found no connected words to left\n"; 
      }

      # find connected word to the right
      for ($right=$i+1; $right<@words && !(exists($outParent{$right})); $right++){}
      if ($right==@words){ 
	$outParent{$i} = 0; 
	#print STDERR "Found no connected words to right\n"; 
      }

      unless (exists($outParent{$i})){
	@ancestors=();
	push(@ancestors, $right);
	while(exists($outParent{$right})){
	  push(@ancestors, $outParent{$right});
	  $right = $outParent{$right};
	}
	$ancestor = $left;
	while(1){
	  foreach $word (@ancestors){
	    if ($word == $ancestor){
	      $outParent{$i} = $ancestor;
	    }
	  }
	  if(exists($outParent{$i})){
	    last;
	  }
	  if (exists($outParent{$ancestor})){
	    $ancestor = $outParent{$ancestor};
	  } else {
	    print STDERR "$lineNum: No common ancestor found for $left and $right\n";
	    last;
	  }
	}
      }
    }
  }
  for ($i=1; $i<@words; $i++){
    if (exists($outParent{$i})){
      $parent=$outParent{$i};
    } else {
      print STDERR "Could not find parent of word $word\n";
    }
    if (exists($outRelation{$i})){
      $relation = $outRelation{$i};
    } else {
      if (($words[$i] eq "and") ||
	  ($words[$i] eq "but") ||
	  ($words[$i] eq "or") ||
	  ($words[$i] eq "however") ||
	  ($words[$i] eq ";")){
	$relation = "conj";
      } else {
	$relation = "unknown";
      }
    }
    if (exists($outTag{$i})){
      $tag = $outTag{$i};
    } else {
      if (($words[$i] eq "and") ||
	  ($words[$i] eq "but") ||
	  ($words[$i] eq "or") ||
	  ($words[$i] eq "however") ||
	  ($words[$i] eq ";")){
	$tag = "conj";
      } else {
	$tag = "unknown";
      }
    }
    if ($for_input==1){
      print "$i $parent\t";
    } else {
      $tag=~s/_/-/g;
      $relation=~s/_/-/g;
#      print $i."_".$tag."_".$relation." ".$parent."\t";
      print $tag."_".$relation." ";
    }
  }
  print "\n";
}
