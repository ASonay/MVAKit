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
#include <thread>

#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "MVAKit/ReadTree.hpp"
#include "MVAKit/Configuration.hpp"

#include "TTree.h"
#include "TFile.h"
#include "TChain.h"

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
  

class MVAKit : public Configuration{
public:
  MVAKit(const char *name);
  ~MVAKit();

  void CloseFile();
  void CloseCSV() {m_csvfile.close();}
  void SetCSV(const char *name);
  void SetFile(const char *name);
  void SetEvents(const char *file="",const char *label="",int doCut=1);
  void SetLoaders(TMVA::DataLoader* loader);
  void SetLoaders(const vector<TMVA::DataLoader*> &loaders);
  void CloneFiles(bool cond=true) {m_fclone=cond; m_fsave=cond;}
  void ExecuteModel();
  int GetNVar() {return m_nvar;}
  int GetNSpectatorVar() {return m_nvarSpec;}


private:
  vector<int> ConvertLabelOpt();
  
  double GetParam(string file,Double_t &weight);
  
  void ReadEvents(string label, vector<string> files);
  void AssignEvents(const string fname);
  bool FillVarstoRecord();
  void SetFiletoClone(string name);
  void WritetoCSV(string type, int split);

  bool is_second;
  bool m_fsave;
  bool m_fclone;
  bool m_fcsv;
  unsigned m_maxth;
  
  vector<thread> m_th;
  
  vector<string> m_activeVars;
  string m_label_current;
  string m_weight_current;
  string m_cut_current;
  string m_tree_current;
  map<string,double> m_scale_current;

  map<string,int> m_LabelEntries;
  map<string,float> m_LabelWeight;

  vector<int> m_classID;
  vector<double> m_weights;
  vector<double> m_param_vec;
  map<int,double> m_param_map;
  map<int,double> m_param_sample_weights;

  double m_total;
  double m_total_param_weight;
  Double_t m_w;
  vector<Double_t> m_vars;
  vector<Double_t> m_varsSpec;
  vector<vector<Double_t>> m_vecs;
  vector<vector<Double_t>> m_vecsSpec;
  unique_ptr<Double_t[]> m_var_rec;
  unique_ptr<vector<Double_t>[]> m_vec_rec;
 
  vector<TMVA::DataLoader*> m_loaders;

  vector<vector<DataContainer>> m_dataTrain;
  vector<vector<DataContainer>> m_dataTest;

  unique_ptr<ReadTree> m_treader;
  vector<unique_ptr<TTree>> m_ttree_test;
  vector<unique_ptr<TTree>> m_ttree_train;
  unique_ptr<TTree> m_ttree;
  unique_ptr<TFile> m_tfile;
  unique_ptr<TChain> m_tchain;

  mt19937 m_gen;
  discrete_distribution<int> m_discDist;

  ofstream m_csvfile;
};
#endif
