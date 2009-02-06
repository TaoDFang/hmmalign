#ifndef HMM_H
#define HMM_H

class HMMParams{
 public:
  virtual void countTransition(int i, int ii, int j, int tm, double count)=0;
  virtual void countEmission(int i, int j, double count)=0;
};

double doBaumWelch(double** emission, 
		   double*** transition, 
		   int numStates, 
		   int numObservations, 
		   int numTransitionModels,
		   double** forward,  
		   double** backward,
		   HMMParams* hmmParams);
int doViterbi(double** emission, 
	      double*** transition, 
	      int numStates, 
	      int numObservations, 
	      int numTransitionModels,
	      double** bestScore,
	      int** backtrace,
	      HMMParams* nextHmmParams);

#endif
