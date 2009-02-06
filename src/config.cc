#include <stdlib.h>
#include <string.h>
#include "config.h"

Configuration::Configuration(FILE* configFile):
  stopAfter(0),
  pruneModel1Dictionary(false),
  pruneHmmDictionary(false),
  hmmDictionaryCutoff(0.0),
  model1DictionaryCutoff(0.0),
  maxBucketSize(0),
  model1Iterations(0),
  model1DumpFreq(0),
  useMooreModel1(false),
  useMooreLLREstimate(false),
  useLLRReestimation(false),
  parsePartitionFeature(-1),
  surfacePartitionFeature(-1),
  lambdaPartitionFeature(-1),
  mooreVocabSize(0),
  mooreEMNW(1),
  mooreAddN(0),
  hmmIterations(0),
  hmmDumpFreq(0),
  printNulls(true), 
  printScores(true), 
  printInOrder(false),
  parseLambda(0.0),
  cacheCorpus(true),
  dumpFirstNAlignments(0),
  useSeparateInitCount(false),
  useSeparateTermCount(false)
{
  bool foundCorpusFile = false;
  bool foundOutputDir = false;
  char key[1000];
  
  int standin;
  
  while (!feof(configFile) &&
	 fscanf(configFile, "%s ", key) == 1){

    if (strcmp(key, "corpusFile")==0){
      fscanf(configFile, "%s", corpusFile);
      fprintf(stderr, "corpusFile = %s\n", corpusFile);
      foundCorpusFile = true;
    } else if (strcmp(key, "outputDirectory")==0){
      fscanf(configFile, "%s", outputDir);
      fprintf(stderr, "outputDirectory = %s\n", outputDir);
      foundOutputDir = true;
    } else if (strcmp(key, "initTTableFile")==0){
      fscanf(configFile, "%s", initTTableFile);
      fprintf(stderr, "initTTableFile = %s\n", initTTableFile);
    } else if (strcmp(key, "pruneModel1Dictionary")==0){
      fscanf(configFile, "%d", &standin); pruneModel1Dictionary = (standin==0 ? false : true);
      fprintf(stderr, "pruneModel1Dictionary = %d\n", pruneModel1Dictionary);
    } else if (strcmp(key, "pruneHmmDictionary")==0){
      fscanf(configFile, "%d", &standin); pruneHmmDictionary = (standin==0 ? false : true);
      fprintf(stderr, "pruneHmmDictionary = %d\n", pruneHmmDictionary);
    } else if (strcmp(key, "printNulls")==0){
      fscanf(configFile, "%d", &standin); printNulls = (standin==0 ? false : true);
      fprintf(stderr, "printNulls = %d\n", printNulls);
    } else if (strcmp(key, "printInOrder")==0){
      fscanf(configFile, "%d", &standin); printInOrder  = (standin==0 ? false : true);
      fprintf(stderr, "printInOrder = %d\n", printInOrder);
    } else if (strcmp(key, "printScores")==0){
      fscanf(configFile, "%d", &standin); printScores = (standin==0 ? false : true);
      fprintf(stderr, "printScores = %d\n", printScores);
    } else if (strcmp(key, "doViterbiOnly")==0){
      fscanf(configFile, "%d", &standin); doViterbiOnly  = (standin==0 ? false : true);
      fprintf(stderr, "doViterbiOnly = %d\n", doViterbiOnly);
    } else if (strcmp(key, "stopAfter")==0){
      fscanf(configFile, "%d", &stopAfter);
      fprintf(stderr, "stopAfter = %d\n", stopAfter);
    } else if (strcmp(key, "model1Iterations")==0){
      fscanf(configFile, "%d", &model1Iterations);
      fprintf(stderr, "model1Iterations = %d\n", model1Iterations);
    } else if (strcmp(key, "model1DumpFrequency")==0){
      fscanf(configFile, "%d", &model1DumpFreq);
      fprintf(stderr, "model1DumpFrequency = %d\n", model1DumpFreq);
    } else if (strcmp(key, "parsePartitionFeature")==0){
      fscanf(configFile, "%d", &parsePartitionFeature);
      fprintf(stderr, "parsePartitionFeature = %d\n", parsePartitionFeature);
    } else if (strcmp(key, "surfacePartitionFeature")==0){
      fscanf(configFile, "%d", &surfacePartitionFeature);
      fprintf(stderr, "surfacePartitionFeature = %d\n", surfacePartitionFeature);
    } else if (strcmp(key, "lambdaPartitionFeature")==0){
      fscanf(configFile, "%d", &lambdaPartitionFeature);
      fprintf(stderr, "lambdaPartitionFeature = %d\n", lambdaPartitionFeature);
    } else if (strcmp(key, "dumpFirstNAlignments")==0){
      fscanf(configFile, "%d", &dumpFirstNAlignments);
      fprintf(stderr, "dumpFirstNAlignments = %d\n", dumpFirstNAlignments);
    } else if (strcmp(key, "useMooreModel1")==0){
      fscanf(configFile, "%d", &standin); useMooreModel1  = (standin==0 ? false : true);
      fprintf(stderr, "useMooreModel1 = %d\n", useMooreModel1);
    } else if (strcmp(key, "useMooreLLREstimate")==0){
      fscanf(configFile, "%d", &standin); useMooreLLREstimate  = (standin==0 ? false : true);
      fprintf(stderr, "useMooreLLREstimate = %d\n", useMooreLLREstimate);
    } else if (strcmp(key, "useLLRReestimation")==0){
      fscanf(configFile, "%d", &standin); useLLRReestimation   = (standin==0 ? false : true);
      fprintf(stderr, "useLLRReestimation = %d\n", useLLRReestimation);
    } else if (strcmp(key, "useInitTTable")==0){
      fscanf(configFile, "%d", &standin); useInitTTable = (standin==0 ? false : true);
      fprintf(stderr, "useInitTTable = %d\n", useInitTTable);
    } else if (strcmp(key, "useSeparateInitCount")==0){
      fscanf(configFile, "%d", &standin); useSeparateInitCount = (standin==0 ? false : true);
      fprintf(stderr, "useSeparateInitCount = %d\n", useSeparateInitCount);
    } else if (strcmp(key, "useSeparateTermCount")==0){
      fscanf(configFile, "%d", &standin); useSeparateTermCount = (standin==0 ? false : true);
      fprintf(stderr, "useSeparateTermCount = %d\n", useSeparateTermCount);
    } else if (strcmp(key, "nonUniformInit")==0){
      fscanf(configFile, "%d", &standin);  nonUniformInit  = (standin==0 ? false : true);
      fprintf(stderr, "nonUniformInit = %d\n", nonUniformInit);
    } else if (strcmp(key, "fixParseLambda")==0){
      fscanf(configFile, "%d", &standin); fixParseLambda = (standin==0 ? false : true);
      fprintf(stderr, "fixParseLambda = %d\n", fixParseLambda);
    } else if (strcmp(key, "useWordFeatures")==0){
      fscanf(configFile, "%d", &standin); useWordFeatures    = (standin==0 ? false : true);
      fprintf(stderr, "useWordFeatures = %d\n", useWordFeatures);
    } else if (strcmp(key, "useFeatureBasedLambdas")==0){
      fscanf(configFile, "%d", &standin); useFeatureBasedLambdas= (standin==0 ? false : true);
      fprintf(stderr, "useFeatureBasedLambdas = %d\n", useFeatureBasedLambdas);
    } else if (strcmp(key, "normDistortion")==0){
      fscanf(configFile, "%d", &standin); normDistortion = (standin==0 ? false : true);
      fprintf(stderr, "normDistortion = %d\n", normDistortion);
    } else if (strcmp(key, "cacheCorpus")==0){
      fscanf(configFile, "%d", &standin); cacheCorpus = (standin==0 ? false : true);
      fprintf(stderr, "cacheCorpus = %d\n", cacheCorpus);
    } else if (strcmp(key, "parseLambda")==0){
      fscanf(configFile, "%lf", &parseLambda);
      fprintf(stderr, "parseLambda = %f\n", parseLambda);
    } else if (strcmp(key, "mooreVocabSize")==0){
      fscanf(configFile, "%lf", &mooreVocabSize);
      fprintf(stderr, "mooreVocabSize = %f\n", mooreVocabSize);
    } else if (strcmp(key, "mooreEMNW")==0){
      fscanf(configFile, "%lf", &mooreEMNW);
      fprintf(stderr, "mooreEMNW = %f\n", mooreEMNW);
    } else if (strcmp(key, "mooreInitNW")==0){
      fscanf(configFile, "%lf", &mooreInitNW);
      fprintf(stderr, "mooreInitNW = %f\n", mooreInitNW);
    } else if (strcmp(key, "mooreAddN")==0){
      fscanf(configFile, "%lf", &mooreAddN);
      fprintf(stderr, "mooreAddN = %f\n", mooreAddN);
    } else if (strcmp(key, "mooreLLRExponent")==0){
      fscanf(configFile, "%lf", &mooreLLRExponent);
      fprintf(stderr, "mooreLLRExponent = %f\n", mooreLLRExponent);
    } else if (strcmp(key, "hmmInitExponent")==0){
      fscanf(configFile, "%lf", &hmmInitExponent);
      fprintf(stderr, "hmmInitExponent = %f\n", hmmInitExponent);
    } else if (strcmp(key, "reestimationLLRExponent")==0){
      fscanf(configFile, "%lf", &reestimationLLRExponent);
      fprintf(stderr, "reestimationLLRExponent = %f\n", reestimationLLRExponent);
    } else if (strcmp(key, "hmmDictionaryCutoff")==0){
      fscanf(configFile, "%lf", &hmmDictionaryCutoff);
      fprintf(stderr, "hmmDictionaryCutoff = %f\n", hmmDictionaryCutoff);
    } else if (strcmp(key, "model1DictionaryCutoff")==0){
      fscanf(configFile, "%lf", &model1DictionaryCutoff);
      fprintf(stderr, "model1DictionaryCutoff = %f\n", model1DictionaryCutoff);
    } else if (strcmp(key, "hmmIterations")==0){
      fscanf(configFile, "%d", &hmmIterations);
      fprintf(stderr, "hmmIterations = %d\n", hmmIterations);
    } else if (strcmp(key, "hmmDumpFrequency")==0){
      fscanf(configFile, "%d", &hmmDumpFreq);
      fprintf(stderr, "hmmDumpFrequency = %d\n", hmmDumpFreq);
    } else if (strcmp(key, "maxBucketSize")==0){
      fscanf(configFile, "%d", &maxBucketSize);
      fprintf(stderr, "maxBucketSize = %d\n", maxBucketSize);
    } else {
      char value[1000];
      fprintf(stderr, "Warning: unknown key, value pair: %s = %s.\n", key, value);
    }
  }
  
  if (!foundCorpusFile){
    fprintf(stderr, "Error: No corpus file specified in configuration.\n");
    exit(0);
  }
  if (!foundOutputDir){
    fprintf(stderr, "Error: No output directory specified in configuration.\n");
    exit(0);
  }
}


