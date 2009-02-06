#include <stdio.h>
#include <math.h>
#include "hmm.h"

int nonEmptyState[200][200];
int numNonEmptyStates[200];

const double CUTOFF=0.0;

//**********************************************************************************************************
// The Baum-Welch algorithm
//**********************************************************************************************************
double doBaumWelch(double** emission, 
		   double*** transition, 
		   int numStates, 
		   int numObservations, 
		   int numTransitionModels,
		   double** forward,
		   double** backward,
		   HMMParams* hmmParams){

  double cutoff = 1.0/(2.0*numObservations);

  // calculate compressed matrix; clean forward/backward matrix
  for (int o=1; o<=numObservations; o++){
    numNonEmptyStates[o]=0;
    for (int s=1; s<=numStates; s++){
      forward[s][o] = 0.0; // should be unnecessary
      backward[s][o] = 0.0; // should be unnecessary

      if (emission[s][o]>CUTOFF){
	nonEmptyState[o][numNonEmptyStates[o]++]=s;
      }
    }
  }

  // calculate forward probabilities
  for (int sidx=0; sidx < numNonEmptyStates[1]; sidx++){
    int s = nonEmptyState[1][sidx];
    forward[s][1] = transition[0][0][s] * emission[s][1];
  }
  for (int o=2; o<=numObservations; o++){
    for (int sidx=0; sidx < numNonEmptyStates[o]; sidx++){
      int s = nonEmptyState[o][sidx];
      for (int ssidx=0; ssidx < numNonEmptyStates[o-1]; ssidx++){
	int ss = nonEmptyState[o-1][ssidx];
	forward[s][o] += forward[ss][o-1]*transition[0][ss][s];
      }
      forward[s][o] *= emission[s][o];
    }
  }

  // calculate backward probabilities
  for (int sidx=0; sidx < numNonEmptyStates[numObservations]; sidx++){
    int s = nonEmptyState[numObservations][sidx];
    backward[s][numObservations] = transition[0][s][numStates+1];
  }
  for (int o=numObservations-1; o>=1; o--){
    for (int sidx=0; sidx < numNonEmptyStates[o]; sidx++){
      int s = nonEmptyState[o][sidx];
      for (int ssidx=0; ssidx < numNonEmptyStates[o+1]; ssidx++){
	int ss = nonEmptyState[o+1][ssidx];
	backward[s][o] += transition[0][s][ss]*emission[ss][o+1]*backward[ss][o+1];
      }
    }
  }

  // calculate total probability of sentence 
  double totalProb = 0.0;
  for (int s=1; s<=numStates; s++){
    totalProb += forward[s][numObservations]*backward[s][numObservations];
  }

  // calculate new emission counts
  for (int o=1; o<=numObservations; o++){
    for (int sidx=0; sidx < numNonEmptyStates[o]; sidx++){
      int s = nonEmptyState[o][sidx];
      double count = forward[s][o] * backward[s][o] / totalProb;
      hmmParams->countEmission(s, o, count);
    }
  }

  // calculate new transition counts

  // initial count
  for (int sidx=0; sidx < numNonEmptyStates[1]; sidx++){
    int s = nonEmptyState[1][sidx];

    double count = transition[0][0][s] * emission[s][1] * backward[s][1] / totalProb;
    for (int m=1; m<=numTransitionModels; m++){
      double modelCount = count * transition[m][0][s] / transition[0][0][s];
      hmmParams->countTransition(0, s, 1, m-1, modelCount);
    }
  }

  // terminal count
  for (int sidx=0; sidx < numNonEmptyStates[numObservations]; sidx++){
    int s = nonEmptyState[numObservations][sidx];

    double count = forward[s][numObservations] * transition[0][s][numStates+1] /totalProb;
    for (int m=1; m<=numTransitionModels; m++){
      double modelCount = count * transition[m][s][numStates+1] / transition[0][s][numStates+1];
      hmmParams->countTransition(s, numStates+1, numObservations+1, m-1, modelCount);
    }
  }

  // s->ss count
  for (int o=2; o<=numObservations; o++){
    for (int sidx=0; sidx < numNonEmptyStates[o-1]; sidx++){
      int s = nonEmptyState[o-1][sidx];

      for (int ssidx=0; ssidx < numNonEmptyStates[o]; ssidx++){
	int ss = nonEmptyState[o][ssidx];

	double count = forward[s][o-1] * transition[0][s][ss] * emission[ss][o] * backward[ss][o] / totalProb;
	for (int m=1; m<=numTransitionModels; m++){
	  double modelCount = count * transition[m][s][ss] / transition[0][s][ss];
	  hmmParams->countTransition(s, ss, o, m-1, modelCount);
	}
      }
    }
  }
  return totalProb;
}



//**********************************************************************************************************
// The Viterbi algorithm
//**********************************************************************************************************
int doViterbi(double** emission, 
	      double*** transition, 
	      int numStates, 
	      int numObservations, 
	      int numTransitionModels,
	      double** bestScore,
	      int** backtrace,
	      HMMParams* nextHmmParams)
{
  // fill out the chart
  for (int s=1; s<=numStates; s++){
    bestScore[s][1] = log( transition[0][0][s] ) + log( emission[s][1] );
  }
  for (int o=2; o<numObservations; o++){
    for (int s=1; s<=numStates; s++){
      bestScore[s][o] = bestScore[1][o-1] + log( transition[0][1][s] ) + log( emission[s][o] );
      backtrace[s][o] = 1;
      for (int ss=2; ss<=numStates; ss++){
	double score = bestScore[ss][o-1] + log( transition[0][ss][s] ) + log( emission[s][o] );
	if (score > bestScore[s][o]){
	  bestScore[s][o] = score;
	  backtrace[s][o] = ss;
	}
      }
    }
  }
  for (int s=1; s<=numStates; s++){
    bestScore[s][numObservations] = bestScore[1][numObservations-1] + log( transition[0][1][s] ) + log( emission[s][numObservations] ) + 
      log( transition[0][s][numStates+1] );
    backtrace[s][numObservations] = 1;
    for (int ss=2; ss <= numStates; ss++){
      double score = bestScore[ss][numObservations-1] + log( transition[0][ss][s] ) + log( emission[s][numObservations] ) +
	log( transition[0][s][numStates + 1] );
      if (score > bestScore[s][numObservations]){
	bestScore[s][numObservations] = score;
	backtrace[s][numObservations] = ss;
      }
    }
  }

  // find the best result
  int bestFinalState = 1;
  double bestFinalScore = bestScore[1][numObservations];
  for (int s=2; s<=numStates; s++){
    if (bestScore[s][numObservations] > bestFinalScore){
      bestFinalScore = bestScore[s][numObservations];
      bestFinalState = s;
    }
  }

  // do counts if needed
  if (nextHmmParams != 0){

    // count transition to final state
    for (int m=1; m<=numTransitionModels; m++){
      double modelCount = transition[m][bestFinalState][numStates+1] / transition[0][bestFinalState][numStates+1];
      nextHmmParams->countTransition(bestFinalState, numStates+1, numObservations+1, m-1, 1);
    }
    nextHmmParams->countEmission(bestFinalState, numObservations, 1);

    // count state to state transitions
    int bestState = bestFinalState;
    for (int o=numObservations-1; o>=1; o--){
      int bestStateAfter = bestState;
      bestState = backtrace[bestStateAfter][o+1];
      
      if (nextHmmParams != 0){
	for (int m=1; m<=numTransitionModels; m++){
	  double modelCount = transition[m][bestState][bestStateAfter] / transition[0][bestState][bestStateAfter];
	  nextHmmParams->countTransition(bestState, bestStateAfter, o+1, m-1, 1);
	}
	nextHmmParams->countEmission(bestState, o, 1);
      }
    }
    
    // count transition from start state
    for (int m=1; m<=numTransitionModels; m++){
      double modelCount = transition[m][0][bestState] / transition[0][0][bestState];
      nextHmmParams->countTransition(0, bestState, 1, m-1, 1);
    }
  }

  return bestFinalState;
}
