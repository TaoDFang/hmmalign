/*
 *  $Id: bitext.cc,v 1.1 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: bitext functionality
 */

#include <stdio.h>
#include "bitext.h"
#include "utilfuncs.cc"

Bitext* getNextBitextFromFile(FILE* in, int numSrcFeatures, int numTrgFeatures){
  int id, sourceLen, targetLen;
  int result = fscanf(in, "%d %d %d\n", &id, &sourceLen, &targetLen);
  //  fprintf(stderr, "%d %d %d %d\n", result, id, sourceLen, targetLen);

  if (result == 3){
    return new Bitext(in, numSrcFeatures, numTrgFeatures, id, sourceLen, targetLen);
  }
 
  return 0;
}


Bitext::Bitext(FILE* in, int numSrcFeatures, int numTrgFeatures,
	       int id, int sourceLen, int targetLen):
  id(id),
  sourceLen(sourceLen),
  targetLen(targetLen)
{
  //  fprintf(stderr, "Reading bitext %d (%d %d)\n", id, sourceLen, targetLen);

  source = new int*[sourceLen];
  target = new int*[targetLen];

  parent = new int[sourceLen+1];
  nextSibling = new int[sourceLen+1];
  prevSibling = new int[sourceLen+1];
  firstChild = new int[sourceLen+1];
  depth = new int[sourceLen+1];
  commonAncestor = new_2DArray(commonAncestor, sourceLen+1, sourceLen+1);

  // a helper array used to fill out the previous arrays.
  int* currentChild = new int[sourceLen+1];

  // init all of these arrays for safety
  for (int i=0; i<=sourceLen; i++){
    nextSibling[i]=0;
    prevSibling[i]=0;
    firstChild[i]=0;
    currentChild[i]=0;
    parent[i]=0;
  }

  for (int i=0; i<sourceLen; i++){
    source[i] = new int[numSrcFeatures];
    
    fscanf(in, "%d", &source[i][0]);
    for (int j=1; j<numSrcFeatures; j++){
      fscanf(in, "_%d", &source[i][j]);
    }

    // debug printing.
//     fprintf(stderr, "%d", source[i][0]);
//     for (int j=1; j<numSrcFeatures; j++){
//       fprintf(stderr, "_%d", source[i][j]);
//     }
//     fprintf(stderr, " ");
  }
  //  fprintf(stderr, "\n");

  for (int i=0; i<targetLen; i++){
    target[i] = new int[numTrgFeatures];
    fscanf(in, "%d", &target[i][0]);
    for (int j=1; j<numTrgFeatures; j++){
      fscanf(in, "_%d", &target[i][j]);
    }

    // debug printing
//     fprintf(stderr, "%d", target[i][0]);
//     for (int j=1; j<numTrgFeatures; j++){
//       fprintf(stderr, "_%d", target[i][j]);
//     }
//     fprintf(stderr, " ");
  }
  //  fprintf(stderr, "\n");

  for (int i=0; i<sourceLen; i++){
    int child;
    fscanf(in, "%d", &child);
    if (child==0){
      //      fprintf(stderr, "Error in bitext %d, parent defined for word 0.\n", id);
    }
    if (parent[child] != 0){
      //      fprintf(stderr, "Error in bitext %d, word %d has multiple parents.\n", id, child);
    }

    // by pigeonhole principle, if neither of the above errors occur,
    // every word will modify something...
    fscanf(in, "%d", &parent[child]);

    //    fprintf(stderr, "%d->%d  ", child, parent[child]);
  }
  //  fprintf(stderr, "\n");

  // fill out the directional pointers
  for (int i=1; i<=sourceLen; i++){
    int par = parent[i];

    if (currentChild[par]==0){
      firstChild[par]=i;
      currentChild[par]=i;
    } else {
      nextSibling[currentChild[par]]=i;
      prevSibling[i]=currentChild[par];
      currentChild[par]=i;
    }
  }
  for (int i=0; i<=sourceLen; i++){
    depth[i]=0;
    for (int ii=0; ii<=sourceLen; ii++){
      commonAncestor[i][ii]=-1;
    }
  }

//   // for debugging, uncommment...
//   for (int i=0; i<=sourceLen; i++){
//     fprintf(stderr, "%d\t%d\t%d\t%d\t%d\n", i, parent[i], firstChild[i], nextSibling[i], prevSibling[i]);
//   }

  delete[] currentChild;
}

Bitext::~Bitext(){

  for (int i=0; i< sourceLen; i++){
    delete[] source[i];
  }
  for (int i=0; i < targetLen; i++){
    delete[] target[i];
  }

  delete[] source;
  delete[] target;
  delete[] parent;
  delete[] prevSibling;
  delete[] nextSibling;
  delete[] firstChild;
  delete[] depth;
  delete_2DArray(commonAncestor, sourceLen+1, sourceLen+1);
}

int Bitext::getId(){
  return id;
}

int Bitext::getSourceWordAt(int i){
  if (i==0) return 0;
  return source[i-1][0];
}

int Bitext::getTargetWordAt(int j){
  if (j==0) return 0;
  return target[j-1][0];
}

int Bitext::getParentOfSrcWordAt(int i){
  return parent[i];
}

int Bitext::getDepthOfSrcWordAt(int i){
  if (!depth[i]){
    int dist = 0;
    int ancestor=i;
    while (ancestor!=0){
      ancestor=parent[ancestor];
      dist++;
    }
    depth[i] = dist;
  }
  return depth[i];
}

int* Bitext::getAncestorListOfSrcWordAt(int i){
  int* iancestors = new int[getDepthOfSrcWordAt(i)];
  int height=0;
  int ancestor = i;
  while (ancestor>0){
    iancestors[height++]=ancestor;
    ancestor=parent[ancestor];
  }
  return iancestors;
}

int Bitext::getCommonAncestorOfSrcWordsAt(int i, int j){
  // if value has been cached, just reuse it.
  if (commonAncestor[i][j]!=-1){
    return commonAncestor[i][j];
  }

  int *ianc = getAncestorListOfSrcWordAt(i);
  int *janc = getAncestorListOfSrcWordAt(j);

  //  fprintf(stderr, "Finding ancestor of words %d, %d: ", i, j);
  int commonAnc=0;
  bool stillCommon=true;
  int iHeight=getDepthOfSrcWordAt(i)-1;
  int jHeight=getDepthOfSrcWordAt(j)-1;
  while (stillCommon){
    if (ianc[iHeight]==janc[jHeight]){
      commonAnc = ianc[iHeight];
      //      fprintf(stderr, "%d ", commonAnc);
      iHeight--; jHeight--;
      if (iHeight<0){
	stillCommon=false;
	//	fprintf(stderr, "iHeight = %d\t", iHeight);
      }
      if (jHeight<0){
	stillCommon=false;
	//	fprintf(stderr, "jHeight = %d\t", jHeight);
      }
    } else {
      stillCommon=false;
      //      fprintf(stderr, "DIFF %d != %d\t", ianc[iHeight], janc[jHeight]);
    }
  }
  //  fprintf(stderr, "COMMON ANC(%d, %d) = %d\n", i, j, commonAnc);
  delete[] ianc;
  delete[] janc;
  commonAncestor[i][j] = commonAnc;
  return commonAnc;
}

int Bitext::getFirstChildOfSrcWordAt(int i){
  return firstChild[i];
}

int Bitext::getNextSiblingOfSrcWordAt(int i){
  return nextSibling[i];
}

int Bitext::getSourceFeatureAt(int i, int whichFeature){
  return source[i-1][whichFeature];
}

int Bitext::getTargetFeatureAt(int j, int whichFeature){
  return source[j-1][whichFeature];
}

int* Bitext::getSourceFeaturesAt(int i, int whichFeatures[], int numFeatures, int data[]){
  for (int k=0; k<numFeatures; k++){
    data[k]=source[i-1][whichFeatures[k]];
  }
  return data;
}

int* Bitext::getTargetFeaturesAt(int j, int whichFeatures[], int numFeatures, int data[]){
  for (int k=0; k<numFeatures; k++){
    data[k]=source[j-1][whichFeatures[k]];
  }
  return data;
}

int Bitext::getFirstPreorderSrcWord(){
  return firstChild[0];
}

int Bitext::getPreorderSrcWordAfter(int i){
  if (firstChild[i]!=0){
    return (firstChild[i]);
  } else {
    while (nextSibling[i]==0 && parent[i]!=0){
      i = parent[i];
    }
    if (nextSibling[i]==0){
      return 0;
    } else {
      return (nextSibling[i]);
    }
  }
}

int Bitext::getSourceLength(){
  return sourceLen;
}

int Bitext::getTargetLength(){
  return targetLen;
}


/*
 * $Log: bitext.cc,v $
 * Revision 1.1  2004/01/01 00:29:34  alopez
 * project reads corpus and processes bitexts
 *
 *
 */
