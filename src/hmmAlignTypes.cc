#include <math.h>
#include "hmmAlignTypes.h"
#include "utilfuncs.cc"

HMM_Basic::HMM_Basic(Configuration& config, 
		     Corpus& corpus, 
		     const int MAXLEN, 
		     TTable* oldDict):
  MAXLEN(MAXLEN),
  numTransitionModels(2),
  useFeatureBasedLambdas(config.useFeatureBasedLambdas),
  partitionFeature(config.lambdaPartitionFeature),
  rankPartitionFeature(partitionFeature > 1 ? corpus.getRankSrcFeature(partitionFeature)+1 : 1),
  transitionModel(new TransitionType*[numTransitionModels]),
  transitionLambda(new_2DArray(transitionLambda, rankPartitionFeature, numTransitionModels)),
  fixLambdas(config.fixParseLambda),
  dict(oldDict),
  distHasBeenSet(false)
{
  dictParamRank[0] = corpus.getRankSrcFeature(0)+1;
  dictParamRank[1] = corpus.getRankTrgFeature(0)+1;

  if (dict){
    dictHasBeenSet=true;
  } else {
    dictHasBeenSet=false;
    dict = new TTable(2, dictParamRank, config.maxBucketSize);
  }

  if (fixLambdas || dict){
    for (int c=0; c<rankPartitionFeature; c++){
      transitionLambda[c][0] = 1 - config.parseLambda;
      transitionLambda[c][1] = config.parseLambda;
    }
    if (useFeatureBasedLambdas && config.parseLambda < 1.0){
      transitionLambda[0][0] = 1.0;
      transitionLambda[0][1] = 0;
    }
  } else {
    for (int c=0; c<rankPartitionFeature; c++){
      transitionLambda[c][0] = 0.0;
      transitionLambda[c][1] = 0.0;
    }
  }

  transitionModel[0] = new DistortionTransition(config, corpus, MAXLEN);
  transitionModel[1] = new TreeTransition(config, corpus, MAXLEN);
}

HMM_Basic::~HMM_Basic(){
  if (dict != 0) delete dict;

  delete_2DArray(transitionLambda, rankPartitionFeature, numTransitionModels);
  for (int m=0; m<numTransitionModels; m++){
    delete transitionModel[m];
  }
  delete[] transitionModel;
}

void HMM_Basic::newArrays(double**& emission, double***& transition,
			  double**& forward, double**& backward,
			  double**& bestScore, int**& backtrace, int& numTM)
{
  emission = new_2DArray(emission, MAXLEN+2, MAXLEN+2);
  transition = new_3DArray(transition, 3, MAXLEN+2, MAXLEN+2);
  forward = new_2DArray(forward, MAXLEN+2, MAXLEN+2);
  backward = new_2DArray(backward, MAXLEN+2, MAXLEN+2);
  bestScore = new_2DArray(bestScore, MAXLEN+2, MAXLEN+2);
  backtrace = new_2DArray(backtrace, MAXLEN+2, MAXLEN+2);
  numTM = numTransitionModels;
}

void HMM_Basic::deleteArrays(double**& emission, double***& transition,
			     double**& forward, double**& backward,
			     double**& bestScore, int**& backtrace)
{
  delete_2DArray(emission, MAXLEN+2, MAXLEN+2);
  delete_3DArray(transition, 3, MAXLEN+2, MAXLEN+2);
  delete_2DArray(forward, MAXLEN+2, MAXLEN+2);
  delete_2DArray(backward, MAXLEN+2, MAXLEN+2);
  delete_2DArray(bestScore, MAXLEN+2, MAXLEN+2);
  delete_2DArray(backtrace, MAXLEN+2, MAXLEN+2);
}

void HMM_Basic::initHMM(Bitext* b, double** emission, double*** transition, int* numStates, int* numObservations)
{
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();
  int t[2];
  
  // emission parameters
  for (int i=0; i<=sourceLen; i++){
    t[0] = (i==0) ? 0 : b->getSourceWordAt(i);

    double normalization = 0.0;
    for (int j=1; j<=targetLen; j++){
      t[1] = b->getTargetWordAt(j);
      emission[i][j] = (dictHasBeenSet)? dict->getStat(t, 2) :  1.0;
      if (isnan(emission[i][j])){ 
	fprintf(stderr, "MAJOR ERROR: t(%d, %d) = %.6g\n", t[0], t[1], emission[i][j]);
      }
      normalization += emission[i][j];
    }

    if (normalization > 0.0){
      for (int j=1; j<=targetLen; j++){
	emission[i][j] /= normalization;
      }
    }
  }
  
  // transition parameters -- initialize submodels
  for (int m=0; m<numTransitionModels; m++){
    transitionModel[m]->initTransitions(b, transition[m+1]);
  }
  // transition from i to ii
  for (int i=0; i<=sourceLen; i++){
    int c = (i>0 && useFeatureBasedLambdas) ? b->getSourceFeatureAt(i,partitionFeature) : 0;
    for (int ii=1; ii<=sourceLen+1; ii++){
      transition[0][i][ii] = 0.0;
      if (ii == sourceLen+1){ c = 0; }
      for (int m=1; m<=numTransitionModels; m++){
	transition[m][i][ii] *= transitionLambda[c][m-1];
	transition[0][i][ii] += transition[m][i][ii];
      }
    }
  }

  *numStates=sourceLen;
  *numObservations=targetLen;
  currentBitext=b;
}

void HMM_Basic::countTransition(int i, int ii, int j, int m, double count){ 
  if (isnan(count) || count==0) return;

  transitionModel[m]->countTransition(currentBitext, i, ii, j, count);

  // optional
  if (!fixLambdas){
    int c = (i>0 && useFeatureBasedLambdas) ? currentBitext->getSourceFeatureAt(i,partitionFeature) : 0;
    if (ii == currentBitext->getSourceLength()+1){ c = 0; }
    transitionLambda[c][m]+=count;
  }
}

void HMM_Basic::countEmission(int i, int j, double count){ 
  int e_i = currentBitext->getSourceWordAt(i);
  int f_j = currentBitext->getTargetWordAt(j);
  double sourceLen = currentBitext->getSourceLength();
  double uniformProb = 1.0/sourceLen;

  if (!isnan(count) &&
      count > 0.0){
    bool betterThanRandom = false;
    if (count >= uniformProb){
      betterThanRandom = true;
    }
    dict->increment(e_i, f_j, count, betterThanRandom);
  }
}

void HMM_Basic::prune(double cutoff){
  dict->prune(cutoff);
}

void HMM_Basic::normalize(){
  //  dict->pruneWithRecount();
  dict->normalize();
  dictHasBeenSet=true;
  distHasBeenSet=true;

  for (int m=0; m<numTransitionModels; m++){
    transitionModel[m]->normalize();
  }
  for (int c=0; c<rankPartitionFeature; c++){
    double normalization = 0.0;
    for (int m=0; m<numTransitionModels; m++){
      normalization += transitionLambda[c][m];
    }
    if (normalization > 0){
      for (int m=0; m<numTransitionModels; m++){
	transitionLambda[c][m] /= normalization;
      }
    }
  }
}

void HMM_Basic::dumpToFile(FILE* aDumpFile, FILE* tDumpFile){

  dict->printTable(tDumpFile);

  fprintf(aDumpFile, "NUMBER OF TRANSITION MODELS: %d\n", numTransitionModels);
  for (int c=0; c<rankPartitionFeature; c++){
    bool printLambdas = false;
    for (int m=0; m<numTransitionModels; m++){
      if (transitionLambda[c][m] > 0) printLambdas = true;
    }
    if (printLambdas){
      fprintf(aDumpFile, "Lambdas(%d) = [", c);
      for (int m=0; m<numTransitionModels-1; m++){
	fprintf(aDumpFile, "%.6g,", transitionLambda[c][m]);
      }
      fprintf(aDumpFile, "%.6g]\n", transitionLambda[c][numTransitionModels-1]);
    }
  }
  fprintf(aDumpFile, "\n");

  for (int m=0; m<numTransitionModels; m++){
    fprintf(aDumpFile, "TRANSITION MODEL %d: ", m+1);
    transitionModel[m]->dumpToFile(aDumpFile);
  }
}

void HMM_Basic::setCurrentBitextForCounting(Bitext* b){
  currentBitext = b;
}

HMM_Basic* selectHMMType(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict){
  return new HMM_Basic(config, corpus, MAXLEN, oldDict);
}


