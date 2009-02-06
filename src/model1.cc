#include <stdio.h>
#include "model1.h"


TTable* runModel1(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict, bool prune, double cutoff){
  // reusable arrays
  double trans[MAXLEN][MAXLEN];
  int e_i, f_j;
  //  int t[2];
  int dictParamRank[2] = { corpus.getRankSrcFeature(0), corpus.getRankTrgFeature(0) };
  int sourceAlignment[MAXLEN][MAXLEN];
  int numSourceAlignments[MAXLEN];
  char dumpFileName[1000];
  FILE* dumpFile = 0;

  for (int iteration=0; iteration<config.model1Iterations; iteration++){
    TTable* newDict = new TTable(2, dictParamRank, config.maxBucketSize);

    bool doDump = false;
    if (config.model1DumpFreq > 0 &&
	(iteration+1) % config.model1DumpFreq == 0){
      doDump = true;
      sprintf(dumpFileName, "%s/results.A1.%02d", config.outputDir, iteration+1);
      dumpFile = fopen(dumpFileName, "w");
      if (!dumpFile){ 
	fprintf(stderr, "Could not open file %s.\n", dumpFileName);
      }
    }

    fprintf(stderr, "Model 1 iteration %d... ", iteration+1);

    corpus.firstBitext();
    while (corpus.hasMoreBitexts()){
      Bitext* b = corpus.getNextBitext();
      int sourceLen = b->getSourceLength();
      int targetLen = b->getTargetLength();
      int id = b->getId();
      if (id%5000==0){ fprintf(stderr, "%d... ", id); fflush(stderr); } 
      for (int i=0; i<=sourceLen; i++){
	numSourceAlignments[i]=0;
      }
      //      if (iteration==0) fprintf(stderr, "Processing bitext %d (%d %d)\n", b->getId(), sourceLen, targetLen);
      for (int j=1; j<=targetLen; j++){
	//	t[1] = b->getTargetWordAt(j);
	f_j = b->getTargetWordAt(j);

	if (oldDict == 0){ // do this step only if we have no starting dictionary.
	  double sum = sourceLen+1;

	  // increment count(f_j|e_0)
	  //	  t[0] = 0;
	  //	  newDict->increment(t, 2, 1/sum);

	  e_i = 0;
	  newDict->increment(e_i, f_j, 1/sum, true);

	  //	  fprintf(stderr, "Counting %d %d\n", t[0], t[1]);

	  // increment count(f_j|e_i)
	  for (int i=1; i<=sourceLen; i++){
	    //	    t[0] = b->getSourceWordAt(i);
	    e_i = b->getSourceWordAt(i);
	    newDict->increment(e_i, f_j, 1/sum, true);
	    //	    fprintf(stderr, "Counting %d %d\n", t[0], t[1]);
	  }
	} else {
	  double bestScore = 0.0;
	  int bestAlignment = 0;
	  
	  // compute sum t(f_j|e)
	  //	  t[0] = 0;
	  //	  trans[0][j]  = oldDict->getStat(t);

	  e_i = 0;
	  trans[0][j]  = oldDict->getStat(e_i, f_j);

	  double sum = trans[0][j];
	  bestScore = trans[0][j];
       
	  for (int i=1; i<=sourceLen; i++){
	    //	    t[0] = b->getSourceWordAt(i);
	    e_i = b->getSourceWordAt(i);
	    //	    trans[i][j] = oldDict->getStat(t);
	    trans[i][j] = oldDict->getStat(e_i, f_j);
	    sum += trans[i][j];
	    if (trans[i][j] > bestScore){
	      bestScore = trans[i][j];
	      bestAlignment = i;
	    }
	  }
	  sourceAlignment[bestAlignment][numSourceAlignments[bestAlignment]++] = j;

	  //	  t[0] = 0;
	  //	  newDict->increment(t, 2, trans[0][j]/sum);

	  e_i=0;
	  double score = trans[0][j]/sum;
	  bool betterThanRandom = (score >= 1.0/(sourceLen+1));
	  newDict->increment(e_i, f_j, score, betterThanRandom);
	  for (int i=1; i<=sourceLen; i++){
	    //	    t[0] = b->getSourceWordAt(i);
	    e_i = b->getSourceWordAt(i);
	    double score = trans[i][j]/sum;
	    bool betterThanRandom = (score >= 1.0/(sourceLen+1));
	    newDict->increment(e_i, f_j, score, betterThanRandom);
	  }
	}
      }
      if (doDump){
	if (config.printNulls){
	  for (int j=0; j<numSourceAlignments[0]; j++){
	    fprintf(dumpFile, "%d %d %d\n", id, 0, sourceAlignment[0][j]);
	  }
	}
	for (int i=1; i<=sourceLen; i++){
	  if (numSourceAlignments[i]==0 &&
	      config.printNulls){
	    fprintf(dumpFile, "%d %d %d\n", id, i, 0);
	  }
	  for (int j=0; j<numSourceAlignments[i]; j++){
	    fprintf(dumpFile, "%d %d %d\n", id, i, sourceAlignment[i][j]);
	  }
	}
      }
    }
    if (prune) { 
      fprintf(stderr, "  Pruning dictionary...\n");
      newDict->prune(cutoff); 
    }
      fprintf(stderr, "  Normalizing dictionary...\n");
    newDict->normalize();
    if (oldDict != 0) { delete oldDict; }
    oldDict = newDict;
    if (doDump){
      fclose(dumpFile);
      sprintf(dumpFileName, "%s/results.t1.%02d", config.outputDir, iteration+1);
      dumpFile = fopen(dumpFileName, "w");
      if (!dumpFile){ 
	fprintf(stderr, "Could not open file %s.\n", dumpFileName);
      }
      oldDict->printTable(dumpFile);
      fclose(dumpFile);
    }
  }
  return oldDict;
}
