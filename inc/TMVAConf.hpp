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

#include "ReadTree.hpp"

using namespace std;


struct Counter{
  unsigned count_test;
  unsigned count_train;
  float weight_test;
  float weight_train;
};
  

class TMVAConf{
public:
  TMVAConf(const string name);
  ~TMVAConf();

  void Parser(int argc,char **argv);
  
  void ReadConf();

  void SetEvents(const vector<TMVA::DataLoader*> &loaders);

  vector<string> GetSplit() {return m_split;}
  vector<string> GetVariables() {return m_variables;}
  vector<string> GetVariablesOther() {return m_variables_other;}
  vector<TString> GetTVariables();
  vector<TString> GetTVariablesOther();
  TString GetTrainingOpt() {return m_trainingOpt;}
  TString GetFactoryOpt() {return m_factoryOpt;}
  TString GetClassifierOpt() {return m_classifierOpt;}
  TString GetSamplingOpt() {return m_samplingOpt;}

private:

  vector<string> comaSep(const string str);
  
  bool CheckDigit(const string str);
  bool isVariableExist(const string var, const vector<string> trvars, vector<string> &activeVars);
  bool StringCompare(const string str1, const string str2);
  
  double FindDigit(const string file,const string var);
  double GetParam(string file,map<int,int> &pmap,mt19937 &gen,discrete_distribution<int> &d);
  
  void ReadEvents(const vector<TMVA::DataLoader*> &loaders, string label, vector<string> files);

  Counter AssignEvents(const vector<TMVA::DataLoader*> &loaders, TTree *tree, const string name, map<int,int> &tmp_param_map);

  TTree* GetTree(TTree *tree, Long64_t max, Long64_t first);
  vector<pair<Long64_t,Long64_t>> TreeSplit(int noe);
  
  vector<string> CheckVars(TTree *tree);
  

  int m_split_per;
  
  bool m_parameterized;
  
  vector<pair<string,vector<string>>> m_ntups;
  vector<pair<string,string>> m_weight;
  vector<pair<string,string>> m_cut;
  vector<string> m_variables;
  vector<string> m_variables_other;
  vector<string> m_split;
  string m_paramvar;
  TString m_trainingOpt;
  TString m_factoryOpt;
  TString m_classifierOpt;
  TString m_samplingOpt;

  string m_conFile;
  string m_loadFile;

  string m_label_current;
  string m_weight_current;
  string m_cut_current;
  
  vector<double> m_weights;
  vector<double> m_param_vec;
  map<int,int> m_param_map;


};
