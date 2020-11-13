#ifndef TMVACONF_HPP
#define TMVACONF_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>

#include "TMVATool/Common.hpp"

using namespace std;



class Configuration{
public:
  Configuration(const char* name);
  ~Configuration();

  void Parser(int argc,char **argv);
  
  void ReadConf();

  void SetConf(string conf) {m_confFile=conf;}
  void SetXML(string xml) {m_xmlFile=Common::StringSep(xml);}
  void isClassification(int b) {m_classification=b;}

  vector<pair<string,vector<string>>> GetFilesName() {return m_ntups;}
  vector<pair<string,map<string,double>>> GetScales() {return m_scale;}
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
  char *GetArchitectureOpt() {return c_architectureOpt;}
  char *GetEngineOpt() {return c_engineOpt;}
  char *GetLabelOpt() {return c_labelOpt;}
  vector<TString> GetTXML();
  vector<TString> GetTVariables();
  vector<TString> GetTVariablesOther();
  string GetParameter() {return m_param;}
  string GetClassName() {return m_cname;}
  string GetParamSplit() {return m_psplit;}
  string GetParamScale() {return m_pscale;}
  int GetNSplit() {return m_nsplit;}
  int GetNLabel() {return m_nlabel;}

protected:
  
  vector<pair<string,vector<string>>> m_ntups;
  vector<pair<string,map<string,double>>> m_scale;
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
  vector<string> m_labels;
  string m_paramvar;
  TString m_trainingOpt;
  TString m_factoryOpt;
  TString m_classifierOpt;
  TString m_samplingOpt;

  string m_psplit;
  string m_pscale;
  
  string m_param;
  string m_cname;
  
  int m_split_per;
  int m_nvar;
  int m_nvarSpec;
  int m_nlabel;
  int m_nsplit;
  
  bool m_parameterized;
  bool m_classification;
 
private:

  map<string,double> ScaleMap(const string str);

  string m_confFile;
  string m_confName;

  char *c_architectureOpt;
  char *c_engineOpt;
  char *c_labelOpt;


};
#endif
