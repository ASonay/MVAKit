#ifndef READTREE_HPP
#define READTREE_HPP
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "TChain.h"
#include "TTree.h"

using namespace std;


class ReadTree
{

public:
  ReadTree();
  ~ReadTree();

  void SetVariables(const string title,vector<pair<string,string>> vars);
  void SetVariables(const string title,map<string,string> vars);
  void SetVariables(const string title,const vector<string> vars);
  void SetVariables(const string title,const string var);
  void SetInputs(const string fname, const string tname, const string cut, const string weight);
  
  void ProcessVariables();

  void ResetVariables();
  
  vector<double> GetInputs(const string label, int index=0);
  double GetWeight(int index=0) {return m_weight[index];}

  long long GetEntries() {return m_entries;}

private:
  
  bool isVariableExist(const string var, const vector<string> trVars, vector<string> &activeVars);
  vector<string> CheckVars(const string cut, const string weight);
  
  long long m_entries;

  string m_var_exp;

  vector<pair<string,string>> m_alias;
  
  map<string,vector<int>> m_label_index;
  map<string,vector<string>> m_vars;
  map<int,double*> m_data;
  double *m_weight;
  
  unique_ptr<TChain> m_chain;
};

#endif
