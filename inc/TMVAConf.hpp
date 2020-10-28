#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>


using namespace std;



class TMVAConf{
public:
  TMVAConf(const string name);
  ~TMVAConf();

  void Parser(int argc,char **argv);
  
  void ReadConf();

  vector<pair<string,vector<string>>> GetFilesName() {return m_ntups;}
  vector<pair<string,string>> GetWeights() {return m_weight;}
  vector<pair<string,string>> GetCuts() {return m_cut;}
  vector<pair<string,string>> GetTrees() {return m_tree;}
  vector<string> GetVariables() {return m_variables;}
  vector<string> GetVariablesOther() {return m_variables_other;}
  vector<string> GetSplit() {return m_split;}
  vector<string> GetXML() {return m_xmlFile;}
  vector<string> GetCond() {return m_cond;}
  string GetParameterVariable() {return m_paramvar;}
  TString GetTrainingOpt() {return m_trainingOpt;}
  TString GetFactoryOpt() {return m_factoryOpt;}
  TString GetClassifierOpt() {return m_classifierOpt;}
  TString GetSamplingOpt() {return m_samplingOpt;}
  vector<TString> GetTXML();
  vector<TString> GetTVariables();
  vector<TString> GetTVariablesOther();
  string GetParameter() {return m_param;}
  string GetClassName() {return m_cname;}

private:

  vector<string> ComaSep(const string str);
  
  vector<pair<string,vector<string>>> m_ntups;
  vector<pair<string,string>> m_weight;
  vector<pair<string,string>> m_cut;
  vector<pair<string,string>> m_tree;
  vector<string> m_variables;
  vector<string> m_variables_other;
  vector<string> m_split;
  vector<string> m_xmlFile;
  vector<string> m_cond;
  vector<string> m_loadFile;
  vector<string> m_loadLib;
  string m_paramvar;
  TString m_trainingOpt;
  TString m_factoryOpt;
  TString m_classifierOpt;
  TString m_samplingOpt;

  string m_confFile;
  string m_confName;

  string m_label_current;
  string m_weight_current;
  string m_cut_current;
  
  string m_param;
  string m_cname;

};
