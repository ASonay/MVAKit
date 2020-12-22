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

#include "MVAKit/Common.hpp"

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
  void isExe(int b) {m_execution=b;}

  vector<string> GetVariables();
  vector<string> GetVariablesOther();

  vector<pair<string,vector<string>>> GetFilesName() {return m_ntups;}
  vector<pair<string,map<string,double>>> GetScales() {return m_scale;}
  vector<pair<string,string>> GetWeights() {return m_weight;}
  vector<pair<string,string>> GetCuts() {return m_cut;}
  vector<pair<string,string>> GetTrees() {return m_tree;}
  vector<string> GetSplit() {return m_split;}
  vector<string> GetXML() {return m_xmlFile;}
  vector<string> GetCond() {return m_cond;}
  string GetParameterVariable() {return m_paramvar;}
  TString GetTrainingOpt() {return m_trainingOpt;}
  TString GetFactoryOpt() {return m_factoryOpt;}
  TString GetClassifierOpt() {return m_classifierOpt;}
  TString GetSamplingOpt() {return m_samplingOpt;}
  void GetArchitectureOpt(char carr[]) {strcpy(carr,string(m_arcOpt).c_str());}
  void GetEngineOpt(char carr[]) {strcpy(carr,string(m_engOpt).c_str());}
  void GetLabelOpt(char carr[]) {strcpy(carr,string(m_labOpt).c_str());}
  void GetLabelName(int index, char carr[]) {strcpy(carr,m_labels[index].c_str());}
  void GetVariableName(int index, char carr[]) {strcpy(carr,m_variables[index].second.c_str());}
  void GetSpectatorVariableName(int index, char carr[]) {strcpy(carr,m_variables_other[index].second.c_str());}
  void GetParamName(char carr[]) {strcpy(carr,m_paramvar.c_str());}
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
  vector<pair<string,string>> m_variables;
  vector<pair<string,string>> m_variables_other;
  vector<string> m_split;
  vector<string> m_xmlFile;
  vector<string> m_cond;
  vector<string> m_loadFile;
  vector<string> m_loadLib;
  vector<string> m_labels;
  vector<string> m_exe;
  string m_paramvar;
  TString m_trainingOpt;
  TString m_arcOpt;
  TString m_engOpt;
  TString m_labOpt;
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
  bool m_execution;

  map<string,string> m_labFromOpt;
 
private:
  map<string,double> ScaleMap(const string str);

  string m_confFile;
  string m_confName;

};
#endif
