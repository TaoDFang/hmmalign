#!/usr/bin/perl -w

$file = "";
while (<>){
  $file .= $_;
}

while ($file =~ s/(<[^>]*>)//g){}
print $file;
