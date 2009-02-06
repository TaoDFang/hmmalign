#include <math.h>
#include "transitions.h"
#include "utilfuncs.cc"

void uniformTransition(Bitext* b, double** transition){
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();

  for (int ii=1; ii<=sourceLen; ii++){
    transition[0][ii] = 1.0/sourceLen;
  }
  for (int i=1; i<=sourceLen; i++){
    for (int ii=1; ii<=sourceLen+1; ii++){
      transition[i][ii] = 1.0/(sourceLen+1);
    }
  }
  transition[0][sourceLen+1] = 0.0;
}

void skewedTransition(Bitext* b, double** transition, double exp){
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();

  for (int ii=1; ii<=sourceLen; ii++){
    transition[0][ii] = 1.0/ii;
  }
  for (int i=1; i<=sourceLen; i++){
    for (int ii=1; ii<=sourceLen+1; ii++){
      double denom = 1;
      if (i>ii) denom = i-ii;
      if (ii>i) denom = ii-i;
      transition[i][ii] = 1.0/pow(denom, exp);
    }
  }
  transition[0][sourceLen+1] = 0.0;
}


DistortionTransition::DistortionTransition(Configuration& config, Corpus& corpus, const int MAXLEN):
  partitionFeature(config.surfacePartitionFeature),
  rankPartitionFeature(partitionFeature>0 ? corpus.getRankSrcFeature(partitionFeature)+1 : 1),
  initCount(new double[MAXLEN]),
  termCount(new double[MAXLEN]),
  distortionCount(new_2DArray(distortionCount, rankPartitionFeature, MAXLEN*2)),
  useInitCount(config.useSeparateInitCount),
  useTermCount(config.useSeparateTermCount),
  useWordFeatures(partitionFeature>0 ? config.useWordFeatures : false),
  nonUniformInit(config.nonUniformInit),
  initExponent(config.hmmInitExponent),
  normalized(false),
  normDistortions(config.normDistortion),
  MAXLEN(MAXLEN)
{
  for (int i=0; i<MAXLEN; i++){
    initCount[i]=0.0;
    termCount[i]=0.0;
  }
  for (int c=0; c<rankPartitionFeature; c++){
    for (int i=0; i<MAXLEN*2; i++){
      distortionCount[c][i]=0.0;
    }
  }
}

DistortionTransition::~DistortionTransition(){
  delete[] initCount;
  delete[] termCount;
  delete_2DArray(distortionCount, rankPartitionFeature, MAXLEN*2);
}

void DistortionTransition::normalize(){
  normalized = true;
}

void DistortionTransition::dumpToFile(FILE* f){
  fprintf(f, "DISTORTION TRANSITION PARAMETERS\n");

  for (int c=0; c<rankPartitionFeature; c++){
    for (int i=0; i<MAXLEN*2; i++){
      if (distortionCount[c][i] > 0.0){
	fprintf(f, "DIST %d %d %.6g\n", c, i-MAXLEN, distortionCount[c][i]);
      }
    }
  }
  
  if (useInitCount){
    fprintf(f, "\n");
    for (int i=0; i<MAXLEN; i++){
      if (initCount[i] > 0.0){
	fprintf(f, "INIT %d %.6g\n", i, initCount[i]);
      }
    }
    fprintf(f, "\n");
  }

  if (useTermCount){
    for (int i=0; i<MAXLEN; i++){
      if (termCount[i] > 0.0){
	fprintf(f, "TERM %d %.6g\n", i, termCount[i]);
      }
    }
  }
}

void DistortionTransition::countTransition(Bitext* b, int i, int ii, int j, double count){
  if (useInitCount && i==0){
    initCount[ii] += count;
  } else if (useTermCount && ii==b->getSourceLength()+1){
    termCount[ii-i] += count;
  } else {
    int c = 0;
    if (i>0 && useWordFeatures){
      c = b->getSourceFeatureAt(i,partitionFeature);
    }
    distortionCount[c][i-ii+MAXLEN] += count;
    if (useWordFeatures && c>0){
      distortionCount[0][i-ii+MAXLEN] += count;
    }
  }
}

void DistortionTransition::initTransitions(Bitext* b, double** transition){
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();
  
  if (normalized){
    transition[0][sourceLen+1] = 0.0; //should never happen

    double normalization = 0.0;
    for (int ii=1; ii <=sourceLen; ii++){
      transition[0][ii] = useInitCount ? initCount[ii] : distortionCount[0][0-ii+MAXLEN];
      normalization += transition[0][ii];
    } 
    if (normalization > 0.0){
      for (int ii=1; ii<=sourceLen; ii++){
	transition[0][ii] /= normalization;
      }
    }

    for (int i=1; i<=sourceLen; i++){
      int c = useWordFeatures ? b->getSourceFeatureAt(i,partitionFeature) : 0;

      normalization = 0.0;
      for (int ii=1; ii<=sourceLen; ii++){
	transition[i][ii] = distortionCount[c][i-ii+MAXLEN];
	normalization += transition[i][ii];
      }
      //      transition[i][sourceLen+1] = useTermCount ? termCount[sourceLen+1-i] : distortionCount[c][i-sourceLen+1+MAXLEN];
      transition[i][sourceLen+1] = useTermCount ? termCount[sourceLen+1-i] : distortionCount[0][i-sourceLen+1+MAXLEN];
      //      normalization += transition[i][sourceLen+1];

      if (normalization > 0.0){
	for (int ii=1; ii<=sourceLen+1; ii++){
	  transition[i][ii] /= normalization;
	}
      }
    }
  } else {
    if (nonUniformInit){
      skewedTransition(b, transition, initExponent);
    } else {
      uniformTransition(b, transition);
    }
  } 
}

/***** TreeTransition class **********************************************************************************************/

void TreeTransition::countTransition(Bitext* b, int i, int ii, int j, double count){
  int sourceLen = b->getSourceLength();

  //  if (i==0 && useInitCount){
  if (i==0){
    int iiDepth = b->getDepthOfSrcWordAt(ii);
    //    initCount[iiDepth] += count;
    initCount[ii] += count;
  }
  //  else if (ii==sourceLen+1 && useTermCount){
  else if (ii==sourceLen+1){
    int iDepth = b->getDepthOfSrcWordAt(i);
    //    termCount[iDepth] += count;
    termCount[sourceLen-i] += count;
  }
  if (i != 0 && ii != b->getSourceLength()+1){
    int ancestor = b->getCommonAncestorOfSrcWordsAt(i, ii);
    int iDepth = b->getDepthOfSrcWordAt(i) - b->getDepthOfSrcWordAt(ancestor);
    int iiDepth = b->getDepthOfSrcWordAt(ii) - b->getDepthOfSrcWordAt(ancestor);
    int direction = (i>ii)?0:1;

    int c = useWordFeatures ? b->getSourceFeatureAt(i, partitionFeature) : 0;

    depthDistortionCount[c][direction][iDepth][iiDepth] += count;
  }
}

void TreeTransition::initTransitions(Bitext* b, double** transition){
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();

  if (normalized){
    double normalization = 0.0;
    for (int ii=1; ii <=sourceLen; ii++){
      int iiDepth = b->getDepthOfSrcWordAt(ii);
      //      transition[0][ii] = useInitCount ? initCount[iiDepth] : 1.0/sourceLen;
      //      transition[0][ii] = useInitCount ? initCount[ii] : 1.0/sourceLen;
      transition[0][ii] = initCount[ii];
      normalization += transition[0][ii];
    }
    if (normalization > 0.0){
      for (int ii=1; ii<=sourceLen; ii++){
	transition[0][ii] /= normalization;
      }
    }
    
    transition[0][sourceLen+1] = 0.0; //should never happen

    for (int i=1; i<=sourceLen; i++){

      normalization = 0.0;
      for (int ii=1; ii<=sourceLen; ii++){
	int ancestor = b->getCommonAncestorOfSrcWordsAt(i, ii);
	int iDepth = b->getDepthOfSrcWordAt(i) - b->getDepthOfSrcWordAt(ancestor);
	int iiDepth = b->getDepthOfSrcWordAt(ii) - b->getDepthOfSrcWordAt(ancestor);
	int direction = (i>ii)?0:1;

	int c = useWordFeatures ? b->getSourceFeatureAt(i, partitionFeature) : 0;

	transition[i][ii] = depthDistortionCount[c][direction][iDepth][iiDepth];
	normalization += transition[i][ii];
	  //	}
      }
      //      if (useTermCount){
      if (true){
	int iDepth = b->getDepthOfSrcWordAt(i);
	transition[i][sourceLen+1] = termCount[sourceLen-i];
	//	transition[i][sourceLen+1] = termCount[iDepth];
      } else {
	transition[i][sourceLen+1] = 1.0/(sourceLen+1);
      }

      if (normalization > 0.0){
	for (int ii=1; ii<=sourceLen; ii++){
	  transition[i][ii] /= normalization;
	}
      }
    }
    
  } else {
    if (nonUniformInit){
      skewedTransition(b, transition, initExponent);
    } else {
      uniformTransition(b, transition);
    }
  }
}

void TreeTransition::normalize(){
  normalized = true;
}

void TreeTransition::dumpToFile(FILE* f){
  fprintf(f, "TREE DEPTH TRANSITION PARAMETERS\n");

  for (int c=0; c<rankPartitionFeature; c++){
    for (int d=0; d<2; d++){
      for (int i=0; i<MAXLEN; i++){
	for (int ii=0; ii<MAXLEN; ii++){
	  if (depthDistortionCount[c][d][i][ii] > 0.0){
	    fprintf(f, "%d %c %d %d %.6g\n", c, (d?'<':'>'), i, ii, depthDistortionCount[c][d][i][ii]);
	  }
	}
      }
    }
  }
  fprintf(f, "\n");
  for (int i=0; i<0; i++){
    if (initCount[i]>0.0){
      fprintf(f, "INIT %d %.6g\n", i, initCount[i]);
    }
  }
  fprintf(f, "\n");
  for (int i=0; i<0; i++){
    if (termCount[i]>0.0){
      fprintf(f, "TERM %d %.6g\n", i, termCount[i]);
    }
  }
}

TreeTransition::TreeTransition(Configuration& config, Corpus& corpus, const int MAXLEN):
  partitionFeature(config.parsePartitionFeature),
  rankPartitionFeature(partitionFeature>0 ? corpus.getRankSrcFeature(partitionFeature)+1 : 1),
  depthDistortionCount(new_4DArray(depthDistortionCount, rankPartitionFeature, 2, MAXLEN, MAXLEN)),
  useInitCount(config.useSeparateInitCount),
  useTermCount(config.useSeparateTermCount),
  useWordFeatures(partitionFeature>0 ? config.useWordFeatures : false),
  nonUniformInit(config.nonUniformInit),
  initExponent(config.hmmInitExponent),
  initCount(new double[MAXLEN]),
  termCount(new double[MAXLEN]),
  normalized(false),
  normDistortions(config.normDistortion),
  MAXLEN(MAXLEN)
{
  for (int i=0; i<MAXLEN; i++){
    initCount[i] = 0.0;
    termCount[i] = 0.0;
  }
  for (int c=0; c<rankPartitionFeature; c++){
    for (int d=0; d<2; d++){
      for (int i=0; i<MAXLEN; i++){
	for (int ii=0; ii<MAXLEN; ii++){
	  depthDistortionCount[c][d][i][ii] = 0.0;
	}
      }
    }
  }
}


TreeTransition::~TreeTransition(){
  delete_4DArray(depthDistortionCount, rankPartitionFeature, 2, MAXLEN, MAXLEN);
  delete[] initCount;
  delete[] termCount;
}



