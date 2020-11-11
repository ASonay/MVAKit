#ifndef TMVATOOL_HPP
#define TMVATOOL_HPP

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
#include "TMVATool/Configuration.hpp"


using namespace std;


struct DataContainer{
  vector<Double_t> Vars;
  vector<Double_t> VarsSpec;
  Double_t Weight;
  string Label;
};
  
struct Counter{
  unsigned count_test;
  unsigned count_train;
  float weight_test;
  float weight_train;
};
  

class TMVATool : public Configuration{
public:
  TMVATool(const char *name);
  ~TMVATool();

  void EvaluateEvents(const vector<Float_t*> &vec_variables, const vector<Float_t*> &vec_variablesSpec, string name, TString method);
  void SetEvents();
  void SetLoaders(TMVA::DataLoader* loader);
  void SetLoaders(const vector<TMVA::DataLoader*> &loaders);
  void SetReaders(TMVA::Reader* reader);
  void SetReaders(const vector<TMVA::Reader*> &readers);
  float GetWeight();
  char *GetLabel(void);
  float *GetVars(void);
  float *GetSpectatorVars(void);
  int NextEvent(char *sample,int split_index,int start=0,int max=-1);
  int GetNVar() {return m_nvar;}
  int GetNSpectatorVar() {return m_nvarSpec;}

private:
  
  bool isVariableExist(const string var, const vector<string> trvars, vector<string> &activeVars);
  
  double GetParam(string file,map<int,double> &pmap,Double_t &weight,mt19937 &gen,discrete_distribution<int> &d);
  
  void ReadEvents(string label, vector<string> files);

  void AssignEvents(TTree *tree, const string name, pair<Long64_t,Long64_t> split, map<int,double> &tmp_param_map, vector<Counter> &c);

  vector<pair<Long64_t,Long64_t>> TreeSplit(int noe);
  
  vector<string> CheckVars(TTree *tree);
 
  string m_label_current;
  string m_weight_current;
  string m_cut_current;
  string m_event_sample;
  int m_event_current;
  int m_split_index;
  map<string,double> m_scale_current;
  
  vector<double> m_weights;
  vector<double> m_param_vec;
  map<int,double> m_param_map;
  map<int,double> m_param_sample_weights;

  double m_total;
  double m_total_param_weight;

  vector<TMVA::DataLoader*> m_loaders;
  vector<TMVA::Reader*> m_readers;

  vector<vector<DataContainer>> m_dataTrain;
  vector<vector<DataContainer>> m_dataTest;
  
  float *vars_r_pt;
  float *varsSpec_r_pt;
  char *label_r_pt;
  
};
#endif
