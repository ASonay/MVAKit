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
  bool isVariableExist(const string var, const vector<string> trvars);
  bool StringCompare(const string str1, const string str2);
  
  double FindDigit(const string file,const string var);
  double GetParam(string file,map<int,int> &pmap,mt19937 &gen,discrete_distribution<int> &d);
  
  void ReadEvents(const vector<TMVA::DataLoader*> &loaders, string label, vector<string> files);

  void CheckVars(TTree *tree);
  

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
  
  vector<double> m_weights;
  vector<double> m_param_vec;
  map<int,int> m_param_map;


};
