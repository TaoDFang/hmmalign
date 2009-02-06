/*
 *  $Id: main.cc,v 1.2 2004/01/01 00:29:34 alopez Exp $
 *  Author: Adam Lopez
 *  Purpose: statistics management class
 */

#ifndef TTABLE_H
#define TTABLE_H

#include <stdio.h>

class Iterator;

class TTable{
 public:
  TTable(int numParams, int* paramRanks, int maxBucketSize=0);
  ~TTable();

  /** read stats from file **/
  void read(FILE*);
  
  /** retrieves the statistic associated with a set of parameters */
  double getStat(int* params, int numParams);
  double getStat(int* params);
  double getStat(int, int);
  double getStat(int);
  
  /** increments the named parameter by value. */
  bool increment(int e, int f, double value, bool betterThanRandom);
  bool increment(int e, int f, double value);

  void printTable(FILE* file);

  void normalize(); 

  void prune();
  void prune(double cutoff);
  Iterator* iterator();
  
 private:

  int V_e;
  int V_f;
  int numBuckets;
  int bucketSize;

  struct Node {
    int j;
    double value;
    bool betterThanRandom;
    Node* bigger;
    Node* smaller;

    Node(int f, bool);
    Node(int f, double v, bool);
    ~Node();
    double getScore(int f);
    void normalize(double denom);
    void print(FILE* f, int);
    Node* prune(double*, double);
  }** t;

  double* sum;
  void pruneHelper(bool, double);

  friend class Iterator;
  friend class TTableEntry;
  friend class INode;
};

class TTableEntry{
 public:
  int gete() const;
  int getf() const;
  double getScore() const;
  void setScore(double score);

 private:
  TTableEntry(int, TTable::Node*, TTable*);
  int e;
  TTable::Node* n;
  TTable* ttable;
  
  friend class Iterator;
};

class INode {
 private: 
  TTable::Node* n;
  INode* prev;
  INode(TTable::Node*, INode*);
  ~INode();

  friend class Iterator;
};

class Iterator {
 public:
  bool hasNext();
  TTableEntry* next();
  ~Iterator();

 private:
  Iterator(TTable* ttable);
  TTable* ttable;
  int currente;
  INode * currentf;

  void advance();

  friend class TTable;
};


#endif
