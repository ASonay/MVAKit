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
  double FindDigit(const string file,const string var);
  
  vector<string> m_variables;
  vector<string> m_variables_other;
  vector<string> m_ntupsSig;
  vector<string> m_ntupsBkg;
  vector<string> m_split;
  string m_paramvar;
  string m_wsig;
  string m_wbkg;
  string m_csig;
  string m_cbkg;
  TString m_trainingOpt;
  TString m_factoryOpt;
  TString m_classifierOpt;
  TString m_samplingOpt;

  string m_conFile;

  bool m_parameterized;

};
