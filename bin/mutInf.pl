#!/usr/bin/perl -w

# calculates mutual information of X,Y in input
# input format should be x, y, f(x, y).

$sum = 0;
while(<>){
  if (/^(\S+)\s+(\S+)\s+([\d\.]+)/){
    $x = $1;
    $y = $2;
    $fxy = $3;
    $xy = "$1 $2";

    # f(x) += f(x,y)
    unless (exists($fx{$x})){ $fx{$x}=0; }
    $fx{$x}+=$fxy;

    # f(y) += f(x,y)
    unless (exists($fy{$y})){ $fy{$y}=0; }
    $fy{$y}+=$fxy;

    # f(x,y) = f(x,y)
    $fxy{$xy} = $fxy;

    # sum_X,Y f(x,y) += f(x,y)
    $sum += $fxy;

  } else {
    die "Format error: $_";
  }
}

$mutinf=0;
foreach $x (sort(keys(%fx))){
  foreach $y (sort(keys(%fy))){
    $xy = "$x $y";
    if (exists($fxy{$xy}) && 
	$fxy{$xy} > 0){
      $px = $fx{$x}/$sum;
      $py = $fy{$y}/$sum;
      $pxy = $fxy{$xy}/$sum;

      if ($px > 0 && 
	  $py > 0){
	$form = $pxy * log($pxy/($px*$py));
	$mutinf += $form;
      } else {
	$form = 0;
      }
#      print "mutinf($x, $y) = $form\n";
    }
  }
}

print "mutual information(X, Y) = $mutinf\n";
