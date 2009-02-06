#ifndef HMM_ALIGNMENT_TYPES_H
#define HMM_ALIGNMENT_TYPES_H

#include "bitext.h"
#include "hmm.h"
#include "config.h"
#include "corpus.h"
#include "ttable.h"
#include "transitions.h"

class HMM_Basic: public HMMParams{
 public:
  void newArrays(double**& emission, double***& transition,
		 double**& forward, double**& backward,
		 double**& bestScore, int**& backtrace, int& numTM);
  void deleteArrays(double**& emission, double***& transition,
		    double**& forward, double**& backward,
		    double**& bestScore, int**& backtrace);
  void initHMM(Bitext* b, double** emission, double*** transition, int* numStates, int* numObservations);
  void countTransition(int i, int ii, int j, int tm, double count);
  void countEmission(int i, int j, double count);
  void dumpToFile(FILE*, FILE*); // one file for transition, one for emission
  void normalize();
  void prune(double);
  void setCurrentBitextForCounting(Bitext* b);

  ~HMM_Basic();


 private:
  HMM_Basic(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict);
  int MAXLEN;

  bool useFeatureBasedLambdas;
  int partitionFeature;
  int rankPartitionFeature;

  int numTransitionModels;
  TransitionType** transitionModel;

  double** transitionLambda;
  
  TTable* dict;
  int dictParamRank[2];
  
  Bitext* currentBitext;

  bool dictHasBeenSet;
  bool distHasBeenSet;
  bool fixLambdas;
  
  friend HMM_Basic* selectHMMType(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict);
};

HMM_Basic* selectHMMType(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict);




#endif
