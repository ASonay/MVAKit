#ifndef READTREE_HPP
#define READTREE_HPP
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <unordered_map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "TChain.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "MVAKit/Configuration.hpp"

using namespace std;


class ReadTree
{

public:
  ReadTree();
  ~ReadTree();

  void SetAlias(const vector<pair<string,string>> &vars);
  void SetVariables(const string &title, const vector<VariableStore> &vars);
  void SetVariables(const string &title, const map<string,string> &vars);
  void SetVariables(const string &title, const vector<string> &vars);
  void SetVariables(const string &title, const string &var);
  void SetFormulas(const string &fname, const string &tname, const string &cut, const string &weight);

  bool NextEvent();
  
  void ProcessVariables();

  void ResetVariables();
  
  vector<double> GetInputs(const string &label);
  vector<vector<double>> GetVectors(const string &label);
  double GetWeight(int index) {return m_weight[index];}
  double GetWeight();

  long long GetEntries() {return m_entries;}

private:
  
  bool isVariableExist(const string &var, const vector<string> &trVars, vector<string> &activeVars);
  vector<string> CheckVars(const string &cut, const string &weight);
  
  long long m_entries;
  long long m_entry;

  vector<pair<string,string>> m_alias_by_request;
  map<string,map<unsigned,vector<pair<string,string>>>> m_alias;
  
  map<string,vector<string>> m_vars;
  map<int,double*> m_data;
  double* m_weight;
  
  unique_ptr<TChain> m_chain;
  unordered_map<string,unique_ptr<TTreeFormula>> m_formula;
  unique_ptr<TTreeFormula> m_formula_weight;
  unique_ptr<TTreeFormula> m_formula_cut;
  
};

#endif
