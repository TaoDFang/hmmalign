#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hmmAlign.h"
#include "hmm.h"
#include "hmmAlignTypes.h"
#include "utilfuncs.h"

void writeAlignment(Bitext* b, FILE* dumpFile, int** backtrace, int bestFinalState, 
		    int* numSourceAlignments, int** sourceAlignment, bool printNulls, bool printInOrder);
void print2DArray(FILE* f, double** arr, int fromX, int toX, int fromY, int toY, bool showSum);
void printBitextData(FILE* f, Bitext *b);

void runHMM(Configuration& config, Corpus& corpus, const int MAXLEN, TTable* dict, bool prune, double cutoff){
  
  // reusable arrays
  double** emission;
  double*** transition;
  
  double** forward;
  double** backward;

  int numStates;
  int numObservations;
  int numTransitionModels;

  double** bestScore;
  int** backtrace;

  int** sourceAlignment = new_2DArray(sourceAlignment, MAXLEN, MAXLEN);
  int* numSourceAlignments = new int[MAXLEN];
  char dumpFileName[1000];

  HMM_Basic* prevHmmParams = selectHMMType(config, corpus, MAXLEN, dict);
  prevHmmParams->newArrays(emission, transition, forward, backward, bestScore, backtrace, numTransitionModels);
    
  for (int iteration=0; iteration < config.hmmIterations; iteration++){
    HMM_Basic* nextHmmParams = selectHMMType(config, corpus, MAXLEN, 0);
    fprintf(stderr, "HMM Iteration %d... ", iteration+1);

    bool doDumpThisIteration = false;
    FILE* dumpFile = 0;
    if ((iteration+1) % config.hmmDumpFreq == 0){
      doDumpThisIteration = true;
      sprintf(dumpFileName, "%s/results.Ahmm.%02d", config.outputDir, iteration+1);
      dumpFile = fopen(dumpFileName, "w");
    }
    
    double corpusLogProb = 0.0;
    int numWords = 0;

    corpus.firstBitext();
    bool doDumpThisBitext=doDumpThisIteration;
    while (corpus.hasMoreBitexts()){
      Bitext* b = corpus.getNextBitext();
      int id = b->getId();
      //      fprintf(stderr, "Aligning sentence %d\n", id);
      //      printBitextData(stderr, b);
      if (id%5000==0){ fprintf(stderr, "%d... ", id); }

      if (doDumpThisBitext){
	if ((config.dumpFirstNAlignments > 0) &&
	    (config.dumpFirstNAlignments < id)){
	  fclose(dumpFile);
	  doDumpThisBitext = false;
	}
      }

      prevHmmParams->initHMM(b, emission, transition, &numStates, &numObservations);
      nextHmmParams->setCurrentBitextForCounting(b);

      if (!config.doViterbiOnly){
	double sentenceProb = doBaumWelch(emission, transition, numStates, numObservations, numTransitionModels, forward, backward, nextHmmParams);
	if (isnan(sentenceProb) || sentenceProb==0){
// 	  fprintf(stderr, "WARNING! SENTENCE PROBABILITY ERROR!\nIF YOU ARE SEEING THIS MESSAGE, SOMETHING HAS GONE WRONG!\n\n");
 	  fprintf(stderr, "   Warning: Pr(%d [%d, %d]) = %.6g\n", id, numStates, numObservations, sentenceProb);

// 	  fprintf(stderr, "EMISSION\n");
// 	  print2DArray(stderr, emission, 0, numStates, 0, numObservations, true);
// 	  fprintf(stderr, "TRANSITION (main)\n");
// 	  print2DArray(stderr, transition[0], 0, numStates, 1, numStates+1, true);
// 	  fprintf(stderr, "TRANSITION (distortion)\n");
// 	  print2DArray(stderr, transition[1], 0, numStates, 1, numStates+1, true);
// 	  fprintf(stderr, "TRANSITION (tree)\n");
// 	  print2DArray(stderr, transition[2], 0, numStates, 1, numStates+1, true);
	  
// 	  fprintf(stderr, "FORWARD\n");
// 	  print2DArray(stderr, forward, 1, numStates, 1, numObservations, true);
// 	  fprintf(stderr, "BACKWARD\n");
// 	  print2DArray(stderr, backward, 1, numStates, 1, numObservations, true);
	} else {
	  corpusLogProb += log(sentenceProb);
	  numWords += b->getTargetLength();
	}
      } 
      if (config.doViterbiOnly || doDumpThisIteration){
	int bestFinalState = doViterbi(emission, transition, numStates, numObservations, numTransitionModels, bestScore, backtrace,
		  (config.doViterbiOnly ? nextHmmParams : 0));
	if (doDumpThisBitext){

	  writeAlignment(b, dumpFile, backtrace, bestFinalState, 
			 numSourceAlignments, sourceAlignment, config.printNulls, config.printInOrder);
	}
      }
    }
    if (doDumpThisBitext){
      fclose(dumpFile);
    }
    fprintf(stderr, "\n");
    
    if (doDumpThisIteration){
      sprintf(dumpFileName, "%s/results.ahmm.%02d", config.outputDir, iteration+1);
      FILE* aDumpFile = fopen(dumpFileName, "w");
      if (aDumpFile==NULL){
	fprintf(stderr, "Could not open file %s for writing.\n", dumpFileName);
	exit(0);
      }
      sprintf(dumpFileName, "%s/results.thmm.%02d", config.outputDir, iteration+1);
      FILE* tDumpFile = fopen(dumpFileName, "w");
      if (tDumpFile==NULL){
	fprintf(stderr, "Could not open file %s for writing.\n", dumpFileName);
	exit(0);
      }
      nextHmmParams->dumpToFile(aDumpFile, tDumpFile);
      fclose(aDumpFile);
      fclose(tDumpFile);
    }
    fprintf(stderr, "  Normalizing dictionary..."); fflush(stderr);
    nextHmmParams->normalize();
    fprintf(stderr, "done\n");
    if (prune) { 
      fprintf(stderr, "  Pruning dictionary..."); fflush(stderr);
      nextHmmParams->prune(cutoff); 
    }

    delete prevHmmParams;
    prevHmmParams = nextHmmParams;

    double perWordPerplexity = corpusLogProb / numWords;
    fprintf(stderr, "   perplexity = %.6g\tper-word perplexity = %.6g\n", corpusLogProb, perWordPerplexity);
  }
  prevHmmParams->deleteArrays(emission, transition, forward, backward, bestScore, backtrace);
  delete prevHmmParams;

  delete[] numSourceAlignments;
  delete_2DArray(sourceAlignment, MAXLEN, MAXLEN);
}


void writeAlignment(Bitext* b, FILE* dumpFile, int** backtrace, int bestFinalState, 
		    int* numSourceAlignments, int** sourceAlignment, bool printNulls, bool printInOrder)
{
  int targetLen = b->getTargetLength();
  int sourceLen = b->getSourceLength();
  int id = b->getId();

  if (printInOrder){
    for (int i=1; i<=sourceLen; i++){
      numSourceAlignments[i] = 0;
    }
  }

//   fprintf(stderr, "\nBACKTRACE\n\t");
//   for (int j=1; j<=targetLen; j++){
//     fprintf(stderr, "%d\t", j);
//   }
//   fprintf(stderr, "\n\n");
//   for (int i=1; i<=sourceLen; i++){
//     fprintf(stderr, "%d\t", i);
//     for (int j=1; j<=targetLen; j++){
//       fprintf(stderr, "%d\t", backtrace[i][j]);
//     }
//     fprintf(stderr, "\n");
//   }
//   fprintf(stderr, "\n\n");
  
  // write stuff from backtrace
  int bestState = bestFinalState;
  for (int j=targetLen; j>0; j--){
//     if (0<bestState && bestState<=sourceLen){
//       if (printInOrder){
// 	sourceAlignment[bestState][numSourceAlignments[bestState]++]=j;
//       } else {
// 	fprintf(dumpFile, "%d %d %d\n", id, bestState, j);
// 	fprintf(stderr, "%d %d %d\n", id, bestState, j);
//       }
//     } else {
//       if (printNulls){
// 	fprintf(dumpFile, "%d %d %d\n", id, 0, j);
// 	fprintf(stderr, "%d %d %d\n", id, 0, j);
//       }
//     }
    fprintf(dumpFile, "%d %d %d\n", id, bestState, j);
    bestState = backtrace[bestState][j];
  }

//   if (printInOrder){
//     for (int i=1; i<=sourceLen; i++){
//       if (printNulls && numSourceAlignments[i]==0){
// 	fprintf(dumpFile, "%d %d %d\n", id, i, 0);
// 	fprintf(stderr, "%d %d %d\n", id, i, 0);
//       }
//       for (int j=0; j<numSourceAlignments[i]; j++){
// 	fprintf(dumpFile, "%d %d %d\n", id, i, sourceAlignment[i][j]);
// 	fprintf(stderr, "%d %d %d\n", id, i, sourceAlignment[i][j]);
//       }
//     }
//   }
}

void print2DArray(FILE* f, double** arr, int fromX, int toX, int fromY, int toY, bool showSum){
  double *sum = new double[toX-fromX+1]; 

  fprintf(f, "\t");
  for (int x=fromX; x<=toX; x++){
    sum[x-fromX]=0.0;
    fprintf(f, "%d\t", x);
  }
  fprintf(f, "\n");
  for (int y=fromY; y<=toY; y++){
    fprintf(f, "%d\t", y);
    for (int x=fromX; x<=toX; x++){
      fprintf(f, "%.2g\t", arr[x][y]);
      sum[x-fromX] += arr[x][y];
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\t");
  for (int x=fromX; x<=toX; x++){
    fprintf(f, "%.2g\t", sum[x-fromX]);
  }
  fprintf(f, "\n\n");

  delete[] sum;
 }

void printBitextData(FILE* f, Bitext *b){
  int sourceLen = b->getSourceLength();
  int targetLen = b->getTargetLength();
  
  fprintf(f, "Bitext %d\n", b->getId());
  // source sentence
  for (int i=1; i<=sourceLen; i++){
    fprintf(f, "%d ", b->getSourceWordAt(i));
  }
  fprintf(f, "\n");

  // target sentences
  for (int j=1; j<=targetLen; j++){
    fprintf(f, "%d ", b->getTargetWordAt(j));
  }
  fprintf(f, "\n");

  // source parse
  for (int i=1; i<=sourceLen; i++){
    fprintf(f, "%d -> %d\t", i, b->getParentOfSrcWordAt(i)); 
  }
  fprintf(f, "\n");

  // source depth
  for (int i=1; i<=sourceLen; i++){
    fprintf(f, "%d : %d\t", i, b->getDepthOfSrcWordAt(i)); 
  }
  fprintf(f, "\n");

  // source ancestry
  for (int i=1; i<=sourceLen; i++){
    int depth = b->getDepthOfSrcWordAt(i);
    int* anc = b->getAncestorListOfSrcWordAt(i);
    for (int d=0; d<depth; d++){
      fprintf(f, "%d -> ", anc[d]);
    }
    fprintf(f, "0\n"); 
    delete[] anc;
  }
  fprintf(f, "\n");

  // print common ancestor table
  fprintf(f, "\t");
  for (int ii=1; ii<=sourceLen; ii++){
    fprintf(f, "%d\t", ii);
  }
  fprintf(f, "\n");
  
  for (int i=1; i<=sourceLen; i++){
    fprintf(f, "%d\t", i);
    for (int ii=1; ii<=sourceLen; ii++){
      fprintf(f, "%d\t", b->getCommonAncestorOfSrcWordsAt(i, ii));
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
}
