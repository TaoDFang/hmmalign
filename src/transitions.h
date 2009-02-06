#ifndef TRANSITIONS_H
#define TRANSITIONS_H

#include <stdio.h>
#include "bitext.h"
#include "config.h"
#include "corpus.h"

class TransitionType{
 public:
  virtual void countTransition(Bitext* b, int i, int ii, int j, double count)=0;
  virtual void initTransitions(Bitext* b, double** transition)=0;
  virtual void normalize()=0;
  virtual void dumpToFile(FILE*)=0;
};

class DistortionTransition : public TransitionType {
 public:
  void countTransition(Bitext* b, int i, int ii, int j, double count);
  void initTransitions(Bitext* b, double** transition);
  void normalize();
  void dumpToFile(FILE*);
  DistortionTransition(Configuration& config, Corpus& corpus, const int MAXLEN);
  ~DistortionTransition();
 private:
  int partitionFeature;
  int rankPartitionFeature;

  double* initCount;
  double* termCount;
  double** distortionCount;

  bool useInitCount;
  bool useTermCount;

  bool normalized;
  bool normDistortions;

  bool useWordFeatures;
  
  bool nonUniformInit;
  double initExponent;

  int MAXLEN;
};

class TreeTransition : public TransitionType {
 public:
  void countTransition(Bitext* b, int i, int ii, int j, double count);
  void initTransitions(Bitext* b, double** transition);
  void normalize();
  void dumpToFile(FILE*);
  TreeTransition(Configuration& config, Corpus& corpus, const int MAXLEN);
  ~TreeTransition();
 private:
  int partitionFeature;
  int rankPartitionFeature;

  double**** depthDistortionCount;
  int*** depthDistortionOccur;

  double* initCount;
  double* termCount;

  bool useInitCount;
  bool useTermCount;

  bool normalized;
  bool normDistortions;

  bool useWordFeatures;

  bool nonUniformInit;
  double initExponent;

  int MAXLEN;
};

#endif
