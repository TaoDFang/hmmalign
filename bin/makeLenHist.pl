#!/usr/bin/perl -w

$max = 0;
while (<>){
  if (/(\d+)\s+(\d+)/){
    $count = $1;
    $len = $2;
    if ($len > $max){ $max = $len; }
    for ($i=$len; $i<1000; $i++){
      unless (exists($lessThan{$i})){ $lessThan{$i} = 0; }
      $lessThan{$i} += $count;
    }
    $count{$len} = $count;
  } else {
    die "Format error: $_";
  }
}

@len = ();
@count = ();
@lessThan = ();
for ($i=0; $i<=$max; $i++){
  if (exists($lessThan{$i})){
    push(@lessThan, $lessThan{$i});
    if (exists($count{$i})){
      push(@count, $count{$i});
    } else {
      push(@count, "0");
    }
    push(@len, $i);
  }
}

print "len = [@len];\n";
print "count = [@count];\n";
print "lessThan = [@lessThan];\n";
