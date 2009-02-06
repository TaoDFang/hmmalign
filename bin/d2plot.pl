#!/usr/bin/perl -w

$sub = "";
$printedX=0;
@x=();
@y=();
while (<>){
  if (/^(\d+)\s+([-\d]+)\s+([-\.\de]+)/){
    $sub = $1;
    $distort = $2;
    $prob = $3;

    push(@x, $distort);
    push(@y, $prob);
  }
  if (/=/ &&
     @x > 0 &&
     @y > 0){
    if ($printedX==0){
      print "@x\n";
      $printedX=1;
    }
    print "@y\n";
    @x=();
    @y=();
  }
}

if ($printedX==0){
  print "@x\n";
  $printedX=1;
}
print "@y\n";
