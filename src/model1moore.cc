#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "model1.h"
#include "utilfuncs.cc"

int binarySearchAndInsert(int* array, int arraySize, int value);
double ll(double a, double b, double c, double d);

TTable* runMooreModel1(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* oldDict, bool prune, double cutoff){
  // reusable arrays
  double** trans = new_2DArray(trans, MAXLEN, MAXLEN);
  int** sourceAlignment = new_2DArray(sourceAlignment, MAXLEN, MAXLEN);
  int* numSourceAlignments = new int[MAXLEN];
  char dumpFileName[1000];
  //  int t[2];
  int e_i, f_j;
  int dictParamRank[2] = { corpus.getRankSrcFeature(0)+1, corpus.getRankTrgFeature(0)+1 };

  FILE* dumpFile = 0;

  const double N = config.mooreAddN;
  const double V = config.mooreVocabSize;
  const double EMNW = config.mooreEMNW;

  bool doDump = false;
  if (config.model1DumpFreq==1){
    doDump = true;
  }

  // iteration 1
  if (oldDict == 0){
    if (config.useMooreLLREstimate){
    
      oldDict = new TTable(2, dictParamRank, config.maxBucketSize);

      int* sourceOccur = new int[dictParamRank[0]];         // number of sentences which contain source word (not number of occurrences!)
      int* targetOccur = new int[dictParamRank[1]];         // number of sentences which contain target word (not number of occurrences!)
      int* targetFreq = new int[dictParamRank[1]];          // number of target word occurrences (used to compute null word initial estimate)

      int allOccur = 0;
      int numTargetWords=0;

      for (int i=0; i<dictParamRank[0]; i++){
	sourceOccur[i] = 0;
      }
      for (int j=0; j<dictParamRank[1]; j++){
	targetOccur[j] = 0;
	targetFreq[j] = 0;
      }
   
      int* sourceBag = new int[MAXLEN];
      int* targetBag = new int[MAXLEN];

      corpus.firstBitext();
      fprintf(stderr, "(Moore) Model 1 iteration 1\n");
      fprintf(stderr, "  Counting coocurrences for Moore initial estimate...");
      while(corpus.hasMoreBitexts()){
	Bitext* b = corpus.getNextBitext();
	int sourceLen = b->getSourceLength();
	int targetLen = b->getTargetLength();

	int id = b->getId();
	if (id%5000==0){ fprintf(stderr, "%d...", id); fflush(stderr); }

	allOccur++;
    
	for (int i=0; i<=sourceLen; i++){
	  sourceBag[i]=0;
	}
	for (int j=0; j<=targetLen; j++){
	  targetBag[j]=0;
	}

	int sourceBagSize=0;
	for (int i=1; i<=sourceLen; i++){
	  sourceBagSize = binarySearchAndInsert(sourceBag, sourceBagSize, b->getSourceWordAt(i));
	}
	int targetBagSize=0;
	for (int j=1; j<=targetLen; j++){
	  targetBagSize = binarySearchAndInsert(targetBag, targetBagSize, b->getTargetWordAt(j));
	  targetFreq[b->getTargetWordAt(j)]++;
	  numTargetWords++;
	}

	for (int i=0; i<sourceBagSize; i++){
	  sourceOccur[sourceBag[i]]++;
	  //	  t[0] = sourceBag[i];
	  e_i = sourceBag[i];
	  for (int j=0; j<targetBagSize; j++){
	    //	    t[1] = targetBag[j];
	    f_j = targetBag[j];
	    oldDict->increment(e_i, f_j, 1.0);
	  }
	}
	for (int j=0; j<targetBagSize; j++){
	  targetOccur[targetBag[j]]++;
	}
      }

      fprintf(stderr, "\n  Computing log-likelihood scores\n");

      // this next loop computes the unnormalized log-likelihood for each pair.
      // the summary log-likelihood is also computed.

      double maxLlSum = 0;
      double* sumLL = new double[dictParamRank[0]];
      for (int i=0; i<dictParamRank[0]; i++){
	sumLL[i]=0;
      }
      if (doDump){
	sprintf(dumpFileName, "%s/results.ll1m", config.outputDir);
	dumpFile = fopen(dumpFileName, "w");
	if (!dumpFile){ 
	  fprintf(stderr, "Could not open file %s.\n", dumpFileName);
	  exit(0);
	}
      }

      Iterator* it = oldDict->iterator();
      while (it->hasNext()){
	TTableEntry* t = it->next();

	int e_i = t->gete();
	int f_j = t->getf();

	double ab = sourceOccur[e_i];
	double ac = targetOccur[f_j];
	double a = t->getScore();
	double b = ab - a;
	double c = ac - a;
	double d = allOccur - a - b - c;
	double total = a + b + c + d;
	
	double stat = pow(ll(a, b, c, d), config.mooreLLRExponent);
	//	t->setScore(a*stat);
	t->setScore(a*stat);

	if (doDump){
	  fprintf(dumpFile, "%d\t%d\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.6f\n", e_i, f_j, a, b, c, d, total, stat);
	}
	    
	sumLL[e_i] += stat;
	if (sumLL[e_i] > maxLlSum){
	  maxLlSum = sumLL[e_i];
	}
	delete t;
      }
      delete it;
      delete[] sumLL;

      // the final loop normalizes log-likelihoods using the maximum summed loglihood per source word.
      fprintf(stderr, "  Computing initial probabilities\n  Normalization Factor %.6f\n", maxLlSum);
      corpus.firstBitext();

      it = oldDict->iterator();
      while (it->hasNext()){
	TTableEntry* t = it->next();

	int e_i = t->gete();
	int f_j = t->getf();

	double ll = t->getScore();
	if (ll > 0){
	  double stat = ll / maxLlSum;
	  t->setScore(stat);
	}
	delete t;
      }
      delete it;

      // compute null-translation probabilities (unigram target probablilities);
      //      t[0]=0;
//       e_i=0;
//       for (f_j=1; f_j<dictParamRank[2]; f_j++){
// 	oldDict->increment(e_i, f_j, (((double) targetFreq[f_j])/((double) numTargetWords)));
// 	if (doDump){
// 	  fprintf(dumpFile, "%d %d %f\n", 0, f_j, (((double) targetFreq[f_j])/((double) numTargetWords)));
// 	}
//       }
      if (doDump){
	fclose(dumpFile);
      }

      delete[] sourceOccur;
      delete[] targetOccur;
      delete[] targetFreq;
      delete[] sourceBag;
      delete[] targetBag;

      // prune if necessary
      if (prune){
	if (cutoff==0){
	  fprintf(stderr, "  Pruning dictionary\n");
	  corpus.firstBitext();
	  while (corpus.hasMoreBitexts()){
	    Bitext *b = corpus.getNextBitext();
	  
	    int sourceLen = b->getSourceLength();
	    int targetLen = b->getTargetLength();
	    
	    
	    for (int j=1; j<=targetLen; j++){
	      f_j = b->getTargetWordAt(j); 
	      
	      double sum=0.0;
	      for (int i=1; i<=sourceLen; i++){
		e_i = b->getSourceWordAt(i);
		trans[i][j] = (oldDict->getStat(e_i, f_j));
		sum+= trans[i][j];
	      }
	      for (int i=1; i<=sourceLen; i++){
		e_i = b->getSourceWordAt(i);
		double count = trans[i][j]/sum;
		bool betterThanRandom = (count >= 1.0/sourceLen) ? true : false;
		oldDict->increment(e_i, f_j, 0.0, betterThanRandom);
	      }
	    }
	  }
	}
	oldDict->prune(cutoff);
      }

    } else {
      // else compute first iteration the normal way.
      oldDict = new TTable(2, dictParamRank, config.maxBucketSize);

      fprintf(stderr, "Model 1 Initialization...");
      corpus.firstBitext();
      while (corpus.hasMoreBitexts()){
	Bitext* b = corpus.getNextBitext();
	int sourceLen = b->getSourceLength();
	int targetLen = b->getTargetLength();
	
	int id = b->getId();
	if (id%5000==0){ fprintf(stderr, "%d...", id); fflush(stderr); }

	double sum = sourceLen+1;
    
	for (int j=1; j<=targetLen; j++){
	  //	  t[1] = b->getTargetWordAt(j);
	  f_j = b->getTargetWordAt(j);

	  // increment count(f_j|e_0)
	  //	  t[0] = 0;
	  e_i = 0;
	  oldDict->increment(e_i, f_j, 1/sum);
	  //	  fprintf(stderr, "Counting %d %d\n", t[0], t[1]);
	
	  // increment count(f_j|e_i)
	  for (int i=1; i<=sourceLen; i++){
	    //	    t[0] = b->getSourceWordAt(i);
	    e_i = b->getSourceWordAt(i);
	    oldDict->increment(e_i, f_j, 1/sum);
	    //	    fprintf(stderr, "Counting %d %d\n", t[0], t[1]);
	  }
	}
      }
      fprintf(stderr, "\n");
    }
  }

  if (doDump){
    sprintf(dumpFileName, "%s/results.t1m.1", config.outputDir);
    dumpFile = fopen(dumpFileName, "w");
    if (!dumpFile){ 
      fprintf(stderr, "Could not open file %s.\n", dumpFileName);
      exit(0);
    }
    oldDict->printTable(dumpFile);
    fclose(dumpFile);
  }

  double* sourceOccur = new double[dictParamRank[0]];
  double* targetOccur = new double[dictParamRank[1]];
  double totalLinks = 0;
  for (int iteration=1; iteration<config.model1Iterations; iteration++){
    TTable* newDict = new TTable(2, dictParamRank, config.maxBucketSize);
    
    for (e_i=0; e_i<dictParamRank[0]; e_i++){
      sourceOccur[e_i]=0;
    }
    for (f_j=0; f_j<dictParamRank[1]; f_j++){
      targetOccur[f_j]=0;
    }
    totalLinks = 0;


    if (config.model1DumpFreq > 0 &&
	(iteration+1) % config.model1DumpFreq == 0){
      doDump = true;
      sprintf(dumpFileName, "%s/results.A1m.%d", config.outputDir, iteration+1);
      dumpFile = fopen(dumpFileName, "w");
      if (!dumpFile){ 
	fprintf(stderr, "Could not open file %s.\n", dumpFileName);
	exit(0);
      }
    }

    fprintf(stderr, "(Moore) Model 1 iteration %d...", iteration+1);

    corpus.firstBitext();
    while (corpus.hasMoreBitexts()){

      Bitext* b = corpus.getNextBitext();

      int id = b->getId();
      if (id%5000==0){ fprintf(stderr, "%d...", id); fflush(stderr); }

      int sourceLen = b->getSourceLength();
      int targetLen = b->getTargetLength();

      for (int i=0; i<=sourceLen; i++){
	numSourceAlignments[i]=0;
      }

      // cache translation values
      for (int i=0; i<=sourceLen; i++){
	e_i = (i==0)? 0 : b->getSourceWordAt(i);

	double sum = 0;

	for (int j=1; j<=targetLen; j++){
	  //	  t[1] = b->getTargetWordAt(j); 
	  f_j = b->getTargetWordAt(j); 
	  
// 	  if (iteration==1 && config.useMooreLLREstimate){
// 	    trans[i][j] = (oldDict->getStat(e_i, f_j));
// 	  } else {
// 	    trans[i][j] = (oldDict->getStat(e_i, f_j) + N) / (oldDict->getStat(e_i) + N*V);
// 	  }

	  double count = oldDict->getStat(e_i, f_j);
	  trans[i][j] = (count==0.0)? 0.0 : (count + N) / (oldDict->getStat(e_i) + N*V);

	  sum += trans[i][j];
	}

// 	// the following bit of code normalizes all the translation probs for
// 	// a particular source word, but it seems to harm performance with the Moore model.
// 	for (int j=1; j<=targetLen; j++){
// 	  trans[i][j] /= sum;
// 	}

      }

      // update counts, compute alignment
      for (int j=1; j<=targetLen; j++){
	//	t[1] = b->getTargetWordAt(j);
	f_j = b->getTargetWordAt(j);
	targetOccur[f_j]++;
	totalLinks++;

	double bestScore = 0.0;
	int bestAlignment = 0;
	  
	// compute sum t(f_j|e)
	//	t[0] = 0;
	e_i = 0;
	trans[0][j] *= EMNW;
	double sum = trans[0][j];
	bestScore = trans[0][j];
	
	for (int i=1; i<=sourceLen; i++){
	  //	  t[0] = b->getSourceWordAt(i);
	  e_i = b->getSourceWordAt(i);
	  sum += trans[i][j];
	  if (trans[i][j] > bestScore){
	    bestScore = trans[i][j];
	    bestAlignment = i;
	  }
	}
	sourceAlignment[bestAlignment][numSourceAlignments[bestAlignment]++] = j;
	
	//	t[0] = 0;
	e_i = 0;
	double count = (sum==0)? 0.0 : trans[0][j]/sum;

	bool betterThanRandom = ((count >= 1.0/sourceLen) ? true : false);
	newDict->increment(e_i, f_j, count, betterThanRandom);
	sourceOccur[0]+=count;
	for (int i=1; i<=sourceLen; i++){
	  //	  t[0] = b->getSourceWordAt(i);
	  e_i = b->getSourceWordAt(i);
	  count = (sum==0.0)? 0.0 : trans[i][j]/sum;
	  bool betterThanRandom = ((count >= 1.0/(sourceLen)) ? true : false);
	  newDict->increment(e_i, f_j, count, betterThanRandom);
	  sourceOccur[e_i] += count;
	}
      }

      // optionally output this alignment
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

    fprintf(stderr, "\n");

    if (prune){
      fprintf(stderr, "  Pruning dictionary...\n");
      newDict->prune(cutoff);
    }

    if (config.useLLRReestimation){
      double* llSum = new double[dictParamRank[0]];
      double maxLLSum = 0;
      int maxLLIndex = 0;
    
      for (int i=0; i<dictParamRank[0]; i++){
	llSum[i]=0;
      }
      if (doDump){
	sprintf(dumpFileName, "%s/results.ll%dm", config.outputDir, iteration+1);
	dumpFile = fopen(dumpFileName, "w");
	if (!dumpFile){ 
	  fprintf(stderr, "Could not open file %s.\n", dumpFileName);
	  exit(0);
	}
      }

      fprintf(stderr, "  Computing log-likehood scores from estimated counts.\n");

      Iterator* it = newDict->iterator();
      while (it->hasNext()){
	TTableEntry* t = it->next();

	int e_i = t->gete();
	int f_j = t->getf();

	double a = t->getScore();
	double ab = sourceOccur[e_i];
	double ac = targetOccur[f_j];
	double b = ab-a;
	double c = ac-a;
	double d = totalLinks - a - b - c;
	double total = a + b + c +d;
	
	double llScore = pow(ll(a, b, c, d), config.reestimationLLRExponent);

	if (doDump){
	  fprintf(dumpFile, "%d\t%d\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4f\n", e_i, f_j, a, b, c, d, total, llScore);
	}
	if (llScore > 0){
	  t->setScore(a*llScore);
	  llSum[0] += llScore;
	  if (llSum[0] > maxLLSum){ maxLLSum = llSum[0]; maxLLIndex=0; }
	} else {
	  t->setScore(0.0);
	}
	delete t;
      }
      delete it;

      if (doDump){
	fclose(dumpFile);
      }
      fprintf(stderr, "  NORMALIZATION FACTOR %d %.6f\n", maxLLIndex, maxLLSum); 
    
      delete[] llSum;
    
      fprintf(stderr, "  Computing normalized probabilities from log-likelihood scores.\n");
      corpus.firstBitext();

      it = newDict->iterator();
      while (it->hasNext()){
	TTableEntry* t = it->next();
	
	e_i = t->gete();
	f_j = t->getf();
	
	double stat = t->getScore();
	if (stat > 0.0){
	  t->setScore(stat/maxLLSum);
	} else {
	  t->setScore(0.0);
	}
      }
    } else { // if not using LLR reestimation
      if (prune){ newDict->prune(cutoff); }
      newDict->normalize();
    }

    delete oldDict;
    oldDict = newDict;
    if (doDump){
      //      fclose(dumpFile);
      sprintf(dumpFileName, "%s/results.t1m.%d", config.outputDir, iteration+1);
      dumpFile = fopen(dumpFileName, "w");
      if (!dumpFile){ 
	fprintf(stderr, "Could not open file %s.\n", dumpFileName);
	exit(0);
      }
      oldDict->printTable(dumpFile);
      fclose(dumpFile);
    }
  }

  delete[] targetOccur;

  delete_2DArray(trans, MAXLEN, MAXLEN);
  delete_2DArray(sourceAlignment, MAXLEN, MAXLEN);
  delete[] numSourceAlignments;

  return oldDict;
}

int binarySearchAndInsert(int* array, int arraySize, int value){
  int upperBound = arraySize;
  int lowerBound = 0;

  if (arraySize == 0 || value > array[arraySize-1]){
    array[arraySize] = value;
    return arraySize+1;
  }

  int midpoint = (upperBound + lowerBound) / 2;

  while (array[midpoint] != value &&
	 lowerBound < upperBound){
    if (array[midpoint] > value){
      upperBound = midpoint;
    } else { // array[midpoint] < value
      lowerBound = midpoint+1;
    }
    
    midpoint = (upperBound + lowerBound) / 2;
  }
  if (array[midpoint] == value) return arraySize; // nothing to insert
  
  // upperBound == lowerBound
  int insertPoint = upperBound;
  
  for (int i=arraySize; i>insertPoint; i--){
    array[i] = array[i-1];
  }
  array[insertPoint]=value;

  return arraySize+1;
}
			   
double ll(double a, double b, double c, double d){
  double total = a+b+c+d;

  double aStat = (a==0) ? 0 : a * log ((a/(a+b))*(total/(a+c)));
  double bStat = (b==0) ? 0 : b * log ((b/(a+b))*(total/(b+d)));
  double cStat = (c==0) ? 0 : c * log ((c/(c+d))*(total/(a+c)));
  double dStat = (d==0) ? 0 : d * log ((d/(c+d))*(total/(b+d)));
  
  return (aStat + bStat + cStat + dStat);
}
