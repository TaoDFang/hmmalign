#ifndef HMM_ALIGN_H
#define HMM_ALIGN_H

#include "config.h"
#include "corpus.h"
#include "ttable.h"

void runHMM(Configuration& config, Corpus& corpus, const int MAXLEN, TTable*, bool prune, double cutoff);

#endif
