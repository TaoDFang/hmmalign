#!/usr/bin/perl -w

@ARGV==2 || die "Usage: $0 <first> <last>\n";

$first = $ARGV[0];
$first =~ /^\d+$/ || die "Error: Arguments <first> and <last> must be positive integers\n";

$last = $ARGV[1];
$last =~ /^\d+$/ || die "Error: Arguments <first> and <last> must be positive integers\n";

while (<STDIN>){
  unless (/(\d+)\s+(\d+)\s+(\d+)/){
    die "Format error: $_\n";
  }
  $id = $1;
  if ($id > $last) { last; }
  if ($id >= $first) { print $_; }
}
