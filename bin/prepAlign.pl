#!/usr/bin/perl

##########################################################
#
# $Id: prepAlign.pl,v 1.1.1.1 2003/12/30 03:41:30 alopez Exp $
#
# Author: Adam Lopez
# Purpose: Preprocessing file for alignment inputs
#
##########################################################

$argCount=0;
$useParseFile=0;
foreach $arg (@ARGV){
  if ($arg eq "-h"){
    printHelp();
    exit(0);
  } else {
    $argCount++;
    if ($argCount==1){
      open(SOURCE, $arg) || die "Could not open file $arg.\n";
    }
    if ($argCount==2){
      open(SOURCEDICT, $arg) || die "Could not open file $arg.\n";
    }
    if ($argCount==3){
      open(TARGET, $arg) || die "Could not open file $arg.\n";
    }
    if ($argCount==4){
      open(TARGETDICT, $arg) || die "Could not open file $arg.\n";
    }
    if ($argCount==5){
      open(PARSE, $arg) || die "Could not open file $arg.\n";
      $useParseFile=1;
    }
  }
}

$argCount == 4 || $argCount==5 || die "Error: wrong number of arguments.  Use -h for more information.\n";

$id=0;
if ($sourceFeatures = <SOURCEDICT>){
  print $sourceFeatures;
} else {
  die "Could not find ranks of source features\n";
}

if ($targetFeatures = <TARGETDICT>){
  print $targetFeatures;
} else {
  die "Could not find ranks of target features\n";
}

while (($source=<SOURCE>) &&
       ($target=<TARGET>)){

  chomp $source;
  chomp $target;

  $id++;
  @source = split(/ /, $source);
  @target = split(/ /, $target);
  $sourceLen = scalar(@source);
  $targetLen = scalar(@target);

  if ($useParseFile){
    if ($sourceParse=<PARSE>){
      chomp $sourceParse;
    }
  } else {
    $sourceParse = "1 0";
    for ($i=2; $i<=$sourceLen; $i++){
      $ip = $i-1;
      $sourceParse = "$sourceParse\t$i $ip";
    }
  }

  $targetParse = "1 0";
  for ($i=2; $i<=$targetLen; $i++){
    $ip = $i-1;
    $targetParse = "$targetParse\t$i $ip";
  }

  print "$id $sourceLen $targetLen\n$source\n$target\n$sourceParse\n";
#  print "$id $sourceLen $targetLen\n$source\n$target\n$sourceParse\n$targetParse\n";
}


sub printHelp
{
    print << "Help";

usage: $0 <sourceFile> <sourceDict> <targetFile> <targetDict> [parseFile]

   <sourcefile>
    the numerified source file

   <sourceDict>
    the source dictionary file (from numerify.pl)

   <targetfile>
    the numerified target file

   <targetDict>
    the target dictionary file (from numerify.pl)

   [parsefile]
    (optional) source dependency parses

Help
}


##########################################################
#
# $log$
#
##########################################################

