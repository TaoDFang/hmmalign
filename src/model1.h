#ifndef MODEL1_H
#define MODEL1_H

#include "config.h"
#include "corpus.h"
#include "ttable.h"

TTable* runModel1(Configuration&, Corpus&, const int MAXLEN, TTable* initialDict, bool prun, double cutoff);
TTable* runMooreModel1(Configuration&, Corpus&, const int MAXLEN, TTable* initialDict, bool prune, double cutoff);

#endif
