#!/usr/bin/perl -w

@ARGV>=4 || die "Usage: $0 <answer file> <isGiza?> <rev?> <variable suffix> [list of files]\n";

$answerFile = $ARGV[0];
$isGiza = $ARGV[1];
$isReversed = $ARGV[2];
$variableSuffix = $ARGV[3];

@files = ();
if (@ARGV>4){
  for ($i=3; $i<@ARGV; $i++){
    push(@files, $ARGV[$i]);
  }
} else {
  while (<STDIN>){
    if (/\s*(\S+)\s*$/){
      push(@files, $1);
    }
  }
}

print "Y$variableSuffix = [ ";
foreach $guessFile (@files){
  if (!defined($kidpid = fork())){
    die "Cannot fork: $!";
  } elsif ($kidpid == 0) {
    if ($isGiza == 1){
      if ($isReversed == 1){
	$revFlag = "-r";
      } else {
	$revFlag = "";
      }
      exec("cat $guessFile | giza2ws $revFlag | getFirstNAlignments 48 484 |renumAnswer.pl > links.$guessFile");
    } else {
      if ($isReversed == 1){
	$rev = "| reverse.pl ";
      } else {
	$rev = "";
      }

      exec("cat $guessFile $rev | getFirstNAlignments 48 484 | renumAnswer.pl > links.$guessFile");
    }
  } else {
    waitpid($kidpid, 0);
  }

  open(AERFILE, "wa_eval_align.pl $answerFile links.$guessFile |");
  while (<AERFILE>){
    if (/AER\s*=\s*([\d\.]+)/){
      print "$1 ";
    }
  }
}
print "]\nX$variableSuffix = [ ";
for ($i=1; $i<=@files; $i++){
  print "$i ";
}
print "]\n";
