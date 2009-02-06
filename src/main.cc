/*
 *  $Id: main.cc,v 1.2 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: alignment algorithm 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitext.h"
#include "corpus.h"
#include "ttable.h"
#include "config.h"
#include "model1.h"
#include "hmm.h"
#include "hmmAlign.h"

// maximum sentence length; actually, one less than this
const int MAXLEN = 105;

// run
int main(int argc, char** argv){

  // step 1: get configuration
  if (argc != 2){
    fprintf(stderr, "Usage: %s <configfile>.  See documentation for more detail.\n", argv[0]);    exit(0);
  }

  FILE* configFile = fopen(argv[1], "r");
  if (configFile==NULL){
    fprintf(stderr, "Error: could not open configuration file %s.\n", argv[0]);
    exit(0);
  }
  Configuration config(configFile);
  fclose(configFile);
  fprintf(stderr, "Finished Reading Configuration.\n");

  // step 2: read the corpus
  Corpus corpus(config.corpusFile, config.cacheCorpus);

  // dictionary
  TTable* dict = 0;

  // read initial t-table into memory
  if (config.useInitTTable){
    FILE* ttableFile = fopen(config.initTTableFile, "r");
    if (ttableFile==NULL){
      fprintf(stderr, "Error: could not open T-Table file %s.\n", config.initTTableFile);
      exit(0);
    }
    fprintf(stderr, "Reading Initial T-Table from file %s.\n", config.initTTableFile);
    int dictParamRank[2] = { corpus.getRankSrcFeature(0)+1, corpus.getRankTrgFeature(0)+1 };
    dict = new TTable(2, dictParamRank, config.maxBucketSize);
    dict->read(ttableFile);
  }

  // Model 1
  if (config.model1Iterations>0){
    if (config.useMooreModel1){
      dict = runMooreModel1(config, corpus, MAXLEN, dict, config.pruneModel1Dictionary, config.model1DictionaryCutoff);
    } else {
      dict = runModel1(config, corpus, MAXLEN, dict, config.pruneModel1Dictionary, config.model1DictionaryCutoff);
    }
  }

  // HMM
  runHMM(config, corpus, MAXLEN, dict, config.pruneHmmDictionary, config.hmmDictionaryCutoff);  
}

/*
 * $Log: main.cc,v $
 * Revision 1.2  2004/01/01 00:29:34  alopez
 * project reads corpus and processes bitexts
 *
 * Revision 1.1.1.1  2003/12/30 03:41:30  alopez
 * initialized project
 *
 */
