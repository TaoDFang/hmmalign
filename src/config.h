#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

struct Configuration{

public:
  Configuration(FILE* configFile);

  // output parameters
  int stopAfter;
  bool printNulls;
  bool printScores;
  bool printInOrder;

  //Dictionary parameters
  bool pruneModel1Dictionary;
  bool pruneHmmDictionary;
  double hmmDictionaryCutoff;
  double model1DictionaryCutoff;
  int maxBucketSize;

  // Model 1 parameters
  int model1Iterations;
  int model1DumpFreq;

  // Moore Model 1 parameters
  bool useMooreModel1;
  bool useMooreLLREstimate;
  bool useLLRReestimation;
  double mooreVocabSize;
  double mooreEMNW;
  double mooreInitNW;
  double mooreAddN;
  double mooreLLRExponent;
  double reestimationLLRExponent;

  // HMM Parameters
  int hmmIterations;
  int hmmDumpFreq;
  bool doViterbiOnly;
  
  bool useSeparateInitCount;
  bool useSeparateTermCount;

  int parsePartitionFeature;
  int surfacePartitionFeature;
  int lambdaPartitionFeature;

  double parseLambda;
  bool fixParseLambda;
  bool normDistortion;

  bool nonUniformInit;
  double hmmInitExponent;
  
  bool useWordFeatures;
  bool useFeatureBasedLambdas;

  // input/output parameters
  bool cacheCorpus;
  char corpusFile[1000];
  char outputDir[1000];

  bool useInitTTable;
  char initTTableFile[1000];

  int dumpFirstNAlignments;
};

#endif
