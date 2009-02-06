#!/usr/bin/perl -w

# renumbers the answer files, starting at id 1.
@ARGV==0 || @ARGV==1 || die "Usage: $0 [n]\n";

$nextId=1;
if (@ARGV==1){
  if ($ARGV[0]=~/^(\d+)$/){
    $nextId=$ARGV[0];
  } else {
    die "Argument n must be a whole number\n";
  }
}

%mapping=();
while (<STDIN>){
  if (/^(\d+)(\s.*)/){
    $id = $1;
    $rest = $2;
    unless (exists($mapping{$id})){
      $mapping{$id} = $nextId;
      $nextId++;
    }
    print "$mapping{$id}$rest\n";
  } else {
    die "Format error: $_";
  }
}
