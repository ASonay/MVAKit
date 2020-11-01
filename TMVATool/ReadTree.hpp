#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TCut.h"
#include "TTreeFormula.h"

using namespace std;

struct NormVars{
  double avg;
  double sig;
};

typedef vector<NormVars> NormFactors;

class ReadTree
{

public:
  ReadTree(string filoc, string finame, string tname, vector<string> var);
  ReadTree(string reader_name, TTree *tr,  vector<string> var);
  ~ReadTree();

  vector<double> GetInput(int n);
  double GetInputSingle(int n);
  NormFactors GetNormFactors(int n);
  int GetNoE() {return m_NoE;}
  int GetConditionalIndex(string cond, int beg);
  void SetSingleVariable(string variable);
  bool GetNext();
  void Close();

private:

  TFile *fi;
  TTree *tree;
  
  vector<string> variables;
  vector<TTreeFormula*> tf;
  TTreeFormula *tf_s;
  TTreeFormula *formx;
  
  string name;

  int m_NoE;

  vector<double> xv;
  vector<double> avg;
  vector<double> sig;
  double xd;

  
};

