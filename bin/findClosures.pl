#!/usr/bin/perl -w

$loopcount=0;
$currentSent = 0;
while (<>){
  if (/(\d+)\s+(\d+)\s+(\d+)(\s+[SP]\s*)?$/){
    $sent = $1;
    $src = $2;
    $trg = $3;

    unless ($currentSent eq $sent){
      %sourceClosure = ();
      %targetClosure = ();
      foreach $srcWord (keys(%sourceAlignments)){
#	print "$currentSent EA $srcWord --> $sourceAlignments{$srcWord}\n";
	$sourceClosure{$srcWord} = $sourceAlignments{$srcWord};
      }
      foreach $trgWord (keys(%targetAlignments)){
#	print "$currentSent FA $trgWord --> $targetAlignments{$trgWord}\n";
	$targetClosure{$trgWord} = $targetAlignments{$trgWord};
      }
      print "\n";

      $change = 1;
      $loop=0;
      while ($change == 1  && $loop < 10){
	$change = 0;
	$loop++;
	foreach $srcWord (keys(%sourceClosure)){
	  @sourceClosureSet = split(/ /, $sourceClosure{$srcWord});
	  foreach $trgWord (split(/ /, $sourceClosure{$srcWord})){
	    foreach $srcCloseWord (split(/ /, $targetClosure{$trgWord})){
	      foreach $trgCloseWord (split(/ /, $sourceClosure{$srcCloseWord})){
		unless ($sourceClosure{$srcWord} =~ /(^| )$trgCloseWord($| )/){
		  push(@sourceClosureSet, $trgCloseWord);
		  $change = 1;
		}
	      }
	    }
	  }
	  $sourceClosure{$srcWord} = join(" ", sort(@sourceClosureSet));
	}
	foreach $trgWord (keys(%targetClosure)){
	  @targetClosureSet = split(/ /, $targetClosure{$trgWord});
	  foreach $srcWord (split(/ /, $targetClosure{$trgWord})){
	    foreach $trgCloseWord (split(/ /, $sourceClosure{$srcWord})){
	      foreach $srcCloseWord (split(/ /, $targetClosure{$trgCloseWord})){
		unless ($targetClosure{$trgWord} =~ /(^| )$srcCloseWord($| )/){
		  push(@targetClosureSet, $srcCloseWord);
		  $change = 1;
		}
	      }
	    }
	  }
	  $targetClosure{$trgWord} = join(" ", sort(@targetClosureSet));
	}
      }

      foreach $srcWord (keys(%sourceAlignments)){
	unless ($sourceAlignments{$srcWord} eq $sourceClosure{$srcWord}){
	  print "$currentSent EA $srcWord --> $sourceAlignments{$srcWord}\n";
	  print "$currentSent EC $srcWord --> $sourceClosure{$srcWord}\n";
	}
      }

      foreach $trgWord (keys(%targetAlignments)){
	unless ($targetAlignments{$trgWord} eq $targetClosure{$trgWord}){
	  print "$currentSent FA $trgWord --> $targetAlignments{$trgWord}\n";
	  print "$currentSent FC $trgWord --> $targetClosure{$trgWord}\n";
	}
      }

      %sourceAlignments = ();
      %targetAlignments = ();
      $currentSent = $sent;
    }
    if (exists($sourceAlignments{$src})){
      @sourceAlignments = split(/ /, $sourceAlignments{$src});
      push(@sourceAlignments, $trg);
      $sourceAlignments{$src} = join(" ", sort(@sourceAlignments));
    } else {
      $sourceAlignments{$src} = $trg;
    }
    if (exists($targetAlignments{$trg})){
      @targetAlignments = split(/ /, $targetAlignments{$trg});
      push(@targetAlignments, $src);
      $targetAlignments{$trg} = join(" ", sort(@targetAlignments));
    } else {
      $targetAlignments{$trg} = $src;
    }
  } else {
#    die "Format error: $_";
  }
}
