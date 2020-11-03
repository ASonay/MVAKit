#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TSystem.h"
#include "TCut.h"
#include "TROOT.h"
#include "TStyle.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "TMVATool/TMVAConf.hpp"
#include "TMVATool/TMVARead.hpp"

using namespace std;

vector<TMVA::Reader*> tmva_reader;
vector<Float_t*> vec_variables;
vector<Float_t*> vec_variablesSpec;
vector<pair<int,int>> cond;
TString method;

vector<pair<int,int>> GetCond(vector<string> cond)
{
  vector<pair<int,int>> x;
  for (auto c : cond){
    string first=c.substr(c.find("%") + 1);
    string second=c.substr(c.find("=") + 2);
    int a=atoi(first.c_str());
    int b=atoi(second.c_str());
    x.push_back(make_pair(a,b));
  }
  reverse(x.begin(),x.end());
  cout << "\nYour Selections:" << endl;
  for (auto v : x){
    cout << "<var>%" << v.first << "==" << v.second << endl;
  }
  return x;
}

void Init(string dir,
	  string xml,
	  string cnf
	  )
{

  string lib = dir + "build/lib/libTMVATool.so";
  if (gSystem->Load(lib.c_str())==0)
    {cout << "Your library, " << lib << " successfully loaded." << endl;}
  else if (gSystem->Load(lib.c_str())==1)
    {cout << "Your library, " << lib << " successfully loaded before." << endl;}
  else{
    cout << "Your library, " << lib << " cannot be loaded." << endl;
    exit(0);
  }
  // Configuration part-----------------------------
  TMVAConf conf("Evaluate");

  conf.SetConf(cnf);
  conf.SetXML(xml);
  conf.ReadConf();

  vector<string> split = conf.GetSplit();cond=GetCond(split);
  vector<TString> variables = conf.GetTVariables();
  vector<TString> variablesSpec = conf.GetTVariablesOther();
  vector<TString> weight_file = conf.GetTXML();
  const int variables_size = variables.size();
  const int variablesSpec_size = variablesSpec.size();
  // Configuration part-----------------------------
  //Book TMVA---------------------------------------
  method = conf.GetClassifierOpt()+" method";
  for (auto wf : weight_file){
    TMVA::Reader* rdr= new TMVA::Reader( "!Color:!Silent" );
    Float_t *local_variables = new Float_t[variables_size];
    Float_t *local_variablesSpec = new Float_t[variablesSpec_size];
    for(int i=0;i<variables_size;i++)
      rdr->AddVariable(string(variables[i]).c_str() , &local_variables[i] );
    for(int i=0;i<variablesSpec_size;i++)
      rdr->AddSpectator(string(variablesSpec[i]).c_str() , &local_variablesSpec[i] );
    vec_variables.push_back(local_variables);
    vec_variablesSpec.push_back(local_variablesSpec);
    rdr->BookMVA( method, wf );
    tmva_reader.push_back(rdr);
  }
}

Double_t GetScore(int en=0,
		  Double_t var1=0,
		  Double_t var2=0,
		  Double_t var3=0,
		  Double_t var4=0,
		  Double_t var5=0,
		  Double_t var6=0,
		  Double_t var7=0,
		  Double_t var8=0,
		  Double_t var9=0,
		  Double_t var10=0,
		  Double_t var11=0,
		  Double_t var12=0,
		  Double_t var13=0,
		  Double_t var14=0,
		  Double_t var15=0,
		  Double_t var16=0,
		  Double_t var17=0
		  )
{
  Double_t score = -999.;

  for (unsigned rsize=0;rsize<tmva_reader.size();rsize++){
    vec_variables[rsize][0]=var1;
    vec_variables[rsize][1]=var2;
    vec_variables[rsize][2]=var3;
    vec_variables[rsize][3]=var4;
    vec_variables[rsize][4]=var5;
    vec_variables[rsize][5]=var6;
    vec_variables[rsize][6]=var7;
    vec_variables[rsize][7]=var8;
    vec_variables[rsize][8]=var9;
    vec_variables[rsize][9]=var10;
    vec_variables[rsize][10]=var11;
    vec_variables[rsize][11]=var12;
    vec_variables[rsize][12]=var13;
    vec_variables[rsize][13]=var14;
    vec_variables[rsize][14]=var15;
    vec_variables[rsize][15]=var16;
    vec_variables[rsize][16]=var17;
  }
  for (unsigned rsize=0;rsize<tmva_reader.size();rsize++){
    if (en%cond[rsize].first==cond[rsize].second)
      score=tmva_reader[rsize]->EvaluateMVA( method );
  }

  return score;
}
