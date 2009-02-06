/*
 *  $Id: corpus.h,v 1.1 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: corpus functionality
 */

#include <stdio.h>
#include "bitext.h"

#ifndef CORPUS_H
#define CORPUS_H

class Corpus{
 public:
  Corpus(char* filename, bool cache);
  ~Corpus();
  void firstBitext();
  Bitext* getNextBitext();
  bool hasMoreBitexts();
  int numBitexts();
  int getNumSrcFeatures();
  int getNumTrgFeatures();
  int getRankSrcFeature(int);
  int getRankTrgFeature(int);

 private:
  bool cacheCorpus;
  FILE* corpusFile;
  char corpusFileName[1000];
  Bitext* b, *prevB;
  

  int numSrcFeatures;
  int numTrgFeatures;
  
  int* rankSrcFeatures;
  int* rankTrgFeatures;

  int count;
  struct Node{
    Bitext* b;
    Node* next;
  };

  Node *first, *next;
};

#endif

/*
 * $Log: corpus.h,v $
 * Revision 1.1  2004/01/01 00:29:34  alopez
 * project reads corpus and processes bitexts
 *
 *
 */
