/*
 *  $Id: bitext.h,v 1.1 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: bitext functionality
 */

#include <stdio.h>

#ifndef BITEXT_H
#define BITEXT_H

class Bitext;

Bitext* getNextBitextFromFile(FILE* in, int numSrcFeatures, int numTrgFeatures);

class Bitext{

  friend Bitext* getNextBitextFromFile(FILE*, int, int);

 public:
  ~Bitext();
  int getId();

  int getSourceLength();
  int getTargetLength();

  int getSourceWordAt(int i);
  int getTargetWordAt(int j);
  
  int getSourceFeatureAt(int i, int whichFeature);
  int getTargetFeatureAt(int j, int whichFeature);
  int* getSourceFeaturesAt(int i, int whichFeatures[], int numFeatures, int data[]);
  int* getTargetFeaturesAt(int j, int whichFeatures[], int numFeatures, int data[]);

  int getFirstPreorderSrcWord();
  int getPreorderSrcWordAfter(int i);

  int getDepthOfSrcWordAt(int i);
  int getCommonAncestorOfSrcWordsAt(int i, int j);
  int* getAncestorListOfSrcWordAt(int i);
  int getParentOfSrcWordAt(int i);
  int getFirstChildOfSrcWordAt(int i);
  int getNextSiblingOfSrcWordAt(int i);
 private:
  Bitext(FILE* in, int numSrcFeatures, int numTrgFeatures, int id, int sourceLen, int targetLen);

  int id;

  int sourceLen;
  int targetLen;
  int** source;
  int** target;

  int* parent;
  int* prevSibling;
  int* nextSibling;
  int* firstChild;
  int* depth;
  int** commonAncestor;
};

#endif

/*
 * $Log: bitext.h,v $
 * Revision 1.1  2004/01/01 00:29:34  alopez
 * project reads corpus and processes bitexts
 *
 *
 */
