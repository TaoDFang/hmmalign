/*
 *  $Id: corpus.cc,v 1.1 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: corpus implementation
 */

#include <stdlib.h>
#include <string.h>
#include "corpus.h"


/** constructor **************************************************/
Corpus::Corpus(char* corpusFileName, bool cacheCorpus):
  cacheCorpus(cacheCorpus),
  corpusFile(fopen(corpusFileName, "r")),
  count(0), 
  next(0),
  first(0),
  b(0),
  prevB(0)
{
  strcpy(this->corpusFileName, corpusFileName);
  
  if (corpusFile==NULL){
    fprintf(stderr, "Error: could not open corpus file %s.\n", corpusFileName);
    exit(0);
  }

  // first, read corpus statistics from the first two lines
  fscanf(corpusFile, "%d ", &numSrcFeatures);
  rankSrcFeatures = new int[numSrcFeatures];
  for (int i=0; i < numSrcFeatures; i++){
    fscanf(corpusFile, "%d ", rankSrcFeatures+i);
  }

  fscanf(corpusFile, "%d ", &numTrgFeatures);
  rankTrgFeatures = new int[numTrgFeatures];
  for (int i=0; i < numTrgFeatures; i++){
    fscanf(corpusFile, "%d ", rankTrgFeatures+i);
  }

  if (cacheCorpus){
    // now read the individual bitexts
    Bitext* b = getNextBitextFromFile(corpusFile, numSrcFeatures, numTrgFeatures);
    while (b){
      //    fprintf(stderr, "Got bitext %d\n", b->getId());
      if (first==0){
	first = new Node();
	next = first;
      } else {
	next->next = new Node();
	next = next->next;
      }
      next->b = b;
      b = getNextBitextFromFile(corpusFile, numSrcFeatures, numTrgFeatures);
    }
    if (next){
      fprintf(stderr, "Read %d bitexts into cache\n", next->b->getId());
    }  
    next = first;
    fclose(corpusFile);
  }
}

/** destructor **************************************************/
Corpus::~Corpus(){
  delete[] rankSrcFeatures;
  delete[] rankTrgFeatures;

  Node* n = first;
  while (n != 0){
    Node* prev = n;
    n = n->next;
    delete prev->b;
    delete prev;
  }
}
  

/** **************************************************/
void Corpus::firstBitext(){
  if (cacheCorpus){
    next = first;
  } else {
    if (next!=0) delete next; next=0;
    if (prevB!=0) delete prevB; prevB=0;
    fclose(corpusFile);
    corpusFile = fopen(corpusFileName, "r");
    
    if (corpusFile==NULL){
      fprintf(stderr, "Error: could not open corpus file %s.\n", corpusFileName);
      exit(0);
    }

    // discard corpus statistics from the first two lines
    int tmp;
    fscanf(corpusFile, "%d ", &tmp);
    for (int i=0; i < numSrcFeatures; i++){
      fscanf(corpusFile, "%d ", &tmp);
    }
    
    fscanf(corpusFile, "%d ", &tmp);
    for (int i=0; i < numTrgFeatures; i++){
      fscanf(corpusFile, "%d ", &tmp);
    }
  }
}


/** **************************************************/
Bitext* Corpus::getNextBitext(){
  if (cacheCorpus){
    if (next==0) return 0;
    
    Node* current = next;
    next = next->next;
    return current->b;
  } else {
    if (prevB!=0) { delete prevB; prevB=0; } // user application done with prevb;
    if (hasMoreBitexts()){
      prevB = b;
      b=0;
      return prevB;
    } else {
      prevB = 0;
      b=0;
      return 0;
    }
  }
}
  
/** **************************************************/
bool Corpus::hasMoreBitexts(){
  if (cacheCorpus){
    return (next != 0);
  } else {
    if (b!=0) { return true; }
    
    b = getNextBitextFromFile(corpusFile, numSrcFeatures, numTrgFeatures);

    if (b==0){ return false; }
    else { return true; }
  }
}

/** **************************************************/
int Corpus::numBitexts(){
  return count;
}


int Corpus::getNumSrcFeatures(){
  return numSrcFeatures;
}

int Corpus::getNumTrgFeatures(){
  return numTrgFeatures;
}

int Corpus::getRankSrcFeature(int i){
  return rankSrcFeatures[i];
}

int Corpus::getRankTrgFeature(int i){
  return rankTrgFeatures[i];
}


/*
 * $Log: corpus.cc,v $
 * Revision 1.1  2004/01/01 00:29:34  alopez
 * project reads corpus and processes bitexts
 *
 *
 */
