#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "TMVA/DataLoader.h"

#include "ReadTree.hpp"

using namespace std;

class TMVAConf{
public:
  TMVAConf();
  ~TMVAConf();

  void Parser(int argc,char **argv, vector<string> &v1, vector<string> &v2, string &file);

  void SetConf(vector<string> ntupsSig,vector<string> ntupsBkg,vector<string> variables,vector<string> split,string wbkg,string wsig,string cbkg,string csig);
  
  void SetEvents(const vector<TMVA::DataLoader*> &loaders);

private:
  vector<string> m_variables;
  vector<string> m_ntupsSig;
  vector<string> m_ntupsBkg;
  vector<string> m_split;
  string m_wsig;
  string m_wbkg;
  string m_csig;
  string m_cbkg;

  
};
