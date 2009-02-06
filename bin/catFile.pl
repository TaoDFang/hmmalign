#!/usr/bin/perl -w

# takes a list of files from stdin and concatenates data on stdout

while (<STDIN>){
  unless (/total \d+/){
    if (/(\S+)\s*$/){
      $file = $1;
      system("cat $file");
    }
  }
}
