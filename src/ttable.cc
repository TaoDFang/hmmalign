#include "ttable.h"

TTable::TTable(int numParams, int* V, int maxBucketSize):
  V_e(V[0]),
  V_f(V[1]),
  t(0),
  sum(new double[V_e])
{
  if (maxBucketSize==0){
    maxBucketSize = V_f;
  }
  numBuckets = V_f/maxBucketSize;
  bucketSize=V_f/numBuckets+1;

  fprintf(stderr, "  Number of buckets = %d\n", numBuckets);
  t = new Node*[V_e*numBuckets];

  for (int i=0; i<V_e; i++){
    for (int j=0; j<numBuckets; j++){
      t[i*numBuckets + j] = 0;
    }
    sum[i] = 0.0;
  }
}

TTable::~TTable(){
  for (int i=0; i<V_e*numBuckets; i++){
    if (t[i]!=0){
      delete t[i];
    }
  }
  delete[] t;
  delete[] sum;
}

/** read stats from file **/
void TTable::read(FILE* file){
  int e,f; double score;
  while (!feof(file)){
    fscanf(file, "%d", &e);
    fscanf(file, "%d", &f);
    if (fscanf(file, "%lf", &score)==1){
      if (!increment(e,f,score)){
	fprintf(stderr, "Internal error: could not increment for params %d %d.\n", e, f);
      }
    }
  }
}

/** retrieves the statistic associated with a set of parameters */
double TTable::getStat(int* params, int numParams){
  return getStat(params);
}

double TTable::getStat(int* params){
  return getStat(params[0], params[1]);
}

double TTable::getStat(int e, int f){
  int f_index = e*numBuckets + f/bucketSize;

  if (e>=V_e){ return 0.0; }
  if (t[f_index]==0){ return 0.0; }

  return t[f_index]->getScore(f);
}

double TTable::getStat(int e){
  return sum[e];
}

/** increments the named parameter by value. */
bool TTable::increment(int e, int f, double value){
  increment(e, f, value, false);
}

bool TTable::increment(int e, int f, double value, bool betterThanRandom){
  if (value<=0.0 && betterThanRandom==false){ return true; }
  if (e>=V_e){
    return false;
  }
  int f_index = e*numBuckets + f/bucketSize;

  sum[e]+= value;
  if (t[f_index]==0){
    t[f_index]=new Node(f, value, betterThanRandom);
  }
  else {
    Node* n = t[f_index];
    while (n->j != f){
      if (n->j<f){
	if (n->bigger == 0){
	  n->bigger = new Node(f, betterThanRandom);
	}
	n = n->bigger;
      } else {
	if (n->smaller == 0){
	  n->smaller = new Node(f, betterThanRandom);
	}
	n = n->smaller;
      }
    }
    n->value+= value;
    if (betterThanRandom){
      n->betterThanRandom = true;
    }
  }
}

void TTable::printTable(FILE* file){
//   for (int i=0; i<V_e; i++){
//     if (t[i]!=0){
//       t[i]->print(file, i);
//     }
//   }

  Iterator* it = iterator();
  while (it->hasNext()){
    TTableEntry* entry = it->next();
    fprintf(file, "%d %d %.6g\n", entry->gete(), entry->getf(), entry->getScore());
    delete entry;
  }
  delete it;
}

void TTable::normalize(){
  for (int i=0; i<V_e; i++){
    if (sum[i]>0.0){
      for (int j=0; j<numBuckets; j++){
	if (t[i*numBuckets+j]!=0){
	  t[i*numBuckets + j]->normalize(sum[i]);
	}
      }
    }
    sum[i]=1;
  }
}

void TTable::prune(double cutoff){
  pruneHelper(true, cutoff);
}
void TTable::prune(){
  pruneHelper(true, 0.0);
}

void TTable::pruneHelper(bool recount, double cutoff){
  for (int i=0; i<V_e; i++){
    for (int j=0; j<numBuckets; j++){
      if (t[i*numBuckets+j]!=0){
	t[i*numBuckets+j] = t[i*numBuckets+j]->prune(recount? &sum[i] : 0, cutoff);
      }
    }
  }
}

Iterator* TTable::iterator(){
  return new Iterator(this);
}

TTable::Node::Node(int j, bool b):
  j(j),
  value(0),
  betterThanRandom(b),
  bigger(0),
  smaller(0)
{}

TTable::Node::Node(int j, double v, bool b):
  j(j),
  value(v),
  betterThanRandom(b),
  bigger(0),
  smaller(0)
{}

TTable::Node::~Node(){
  if (bigger != 0) delete bigger; 
  if (smaller != 0) delete smaller;
}

void TTable::Node::normalize(double denom){
  value /= denom;
  if (bigger!=0){ bigger->normalize(denom); }
  if (smaller!=0){ smaller->normalize(denom); }
}

double TTable::Node::getScore(int f){
  if (j==f) return value;
  if (j<f){
    if (bigger!=0){ return bigger->getScore(f); }
    else { return 0.0; }
  } else {
    if (smaller!=0){ return smaller->getScore(f); }
    else { return 0.0; }
  }
}

void TTable::Node::print(FILE* f, int i){
  if (smaller!=0){ smaller->print(f, i); }
  fprintf(f, "%d %d %.6g\n", i, j, value);
  if (bigger!=0){ bigger->print(f, i); }
}

TTable::Node* TTable::Node::prune(double* sum, double cutoff){
  if (smaller!=0){ smaller = smaller->prune(sum, cutoff); }
  if (bigger!=0){ bigger = bigger->prune(sum, cutoff); }

  if ((cutoff==0.0 && !betterThanRandom) ||
      (cutoff>0.0 && cutoff>value)){ 
    if (sum!=0){ *sum -= value; }
    value = 0.0; 
    if (smaller == 0){
      Node* result = bigger;
      bigger = 0; smaller = 0; delete this;
      return result;
    } else if (bigger == 0){
      Node* result = smaller;
      bigger = 0; smaller = 0; delete this;
      return result;
    } else {
      Node* result = smaller;
      Node* leftmost = smaller;
      while (leftmost->bigger!=0){ leftmost = leftmost->bigger; }
      leftmost->bigger = bigger;
      bigger = 0; smaller = 0; delete this;
      return result;
    }
  };
  return this;
}

int TTableEntry::gete() const{
  return e;
}

int TTableEntry::getf() const{
  return n->j;
}

double TTableEntry::getScore() const{
  return n->value;
}

void TTableEntry::setScore(double score){
  ttable->sum[e] += score - n->value;
  n->value = score;
}

TTableEntry::TTableEntry(int e, TTable::Node* n, TTable* t):
  e(e),
  n(n),
  ttable(t){}

INode::INode(TTable::Node* n, INode* prev):
  n(n),
  prev(prev){
//     fprintf(stderr, "     INode(%d)", n->j);
//     if (prev!=0){ fprintf(stderr, "->(%d)\n", prev->n->j); }
//     else { fprintf(stderr, "->|\n"); }
}

INode::~INode(){
  //  fprintf(stderr, "     ~INode(%d)\n", n->j);
}

Iterator::Iterator(TTable* ttable):
  ttable(ttable),
  currente(0),
  currentf(0)
{
  advance();
}

bool Iterator::hasNext(){
  if (currentf==0){ return false; }
  return true;
}

void Iterator::advance(){
  //  fprintf(stderr, "  %d < %d\n", currente, ttable->V_e);
  while(ttable->t[currente]==0 && currente < ttable->V_e * ttable->numBuckets){ 
    currente++;   
    // fprintf(stderr, "  %d < %d\n", currente, ttable->V_e); 
  }
  if (currente < ttable->V_e * ttable->numBuckets){
    TTable::Node* n = ttable->t[currente];
    currentf = new INode(n, 0);
    while (n->smaller!=0){
      n = n->smaller;
      currentf = new INode(n, currentf);
    }
  }
}

TTableEntry* Iterator::next(){
  if (currentf==0){ return 0; }

  TTable::Node* n = currentf->n;
  TTableEntry* result = new TTableEntry(currente/ttable->numBuckets, n, ttable);
  
  INode* tmp = currentf;
  currentf = currentf->prev;

  n = tmp->n->bigger;
  if (n!=0){
    currentf = new INode(n, currentf);
    while (n->smaller!=0){
      n = n->smaller;
      currentf = new INode(n, currentf);
    }
  }
  if (currentf==0){
    currente++;
    advance();
  }

  delete tmp;

  return result;
}

Iterator::~Iterator(){
  while (currentf!=0){
    INode* tmp = currentf;
    currentf = currentf->prev;
    delete tmp;
  }
}
