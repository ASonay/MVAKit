#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>

#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "TMVATool/ReadTree.hpp"

using namespace std;


struct Counter{
  unsigned count_test;
  unsigned count_train;
  float weight_test;
  float weight_train;
};
  

class TMVARead{
public:
  TMVARead(TMVAConf &c);
  ~TMVARead();

  void SetEvents(const vector<TMVA::DataLoader*> &loaders);
  void EvaluateEvents(const vector<TMVA::Reader*> &tmva_reader, const vector<Float_t*> &vec_variables, const vector<Float_t*> &vec_variablesSpec, string name, TString method);

private:
  
  bool CheckDigit(const string str);
  bool isVariableExist(const string var, const vector<string> trvars, vector<string> &activeVars);
  bool StringCompare(const string str1, const string str2);
  
  double FindDigit(const string file,const string var);
  double GetParam(string file,map<int,double> &pmap,Double_t weight,Double_t &weight_param,mt19937 &gen,discrete_distribution<int> &d);
  
  void ReadEvents(const vector<TMVA::DataLoader*> &loaders, string label, vector<string> files);

  void AssignEvents(const vector<TMVA::DataLoader*> &loaders, TTree *tree, const string name, pair<Long64_t,Long64_t> split, map<int,double> &tmp_param_map, vector<Counter> &c);

  vector<pair<Long64_t,Long64_t>> TreeSplit(int noe);
  
  vector<string> CheckVars(TTree *tree);
  

  int m_split_per;
  
  bool m_parameterized;
  
  vector<pair<string,vector<string>>> m_ntups;
  vector<pair<string,string>> m_weight;
  vector<pair<string,string>> m_cut;
  vector<pair<string,string>> m_tree;
  vector<string> m_variables;
  vector<string> m_variables_other;
  vector<string> m_split;
  vector<string> m_xmlFile;
  vector<string> m_cond;
  string m_paramvar;
  string m_psplit;

  string m_label_current;
  string m_weight_current;
  string m_cut_current;
  
  vector<double> m_weights;
  vector<double> m_param_vec;
  map<int,double> m_param_map;
  map<int,double> m_param_sample_weights;

  string m_param;
  string m_cname;

  double m_total;
  double m_total_param_weight;

};
