
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

int main(int argc,char **argv)
{
  // Configuration part-----------------------------
  TMVAConf conf("Evaluate");
    
  conf.Parser(argc,argv);
  conf.ReadConf();
  
  vector<string> split = conf.GetSplit();
  vector<TString> variables = conf.GetTVariables();
  vector<TString> variablesSpec = conf.GetTVariablesOther();
  vector<TString> weight_file = conf.GetTXML();
  const int variables_size = variables.size();
  const int variablesSpec_size = variablesSpec.size();
  // Configuration part-----------------------------
  //Book TMVA---------------------------------------
  TString method = conf.GetClassifierOpt()+" method";
  vector<TMVA::Reader*> tmva_reader;
  vector<Float_t*> vec_variables;
  vector<Float_t*> vec_variablesSpec;
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
  //Book TMVA---------------------------------------
  //Evaluate TMVA-----------------------------------
  TMVARead treader(conf);
  treader.EvaluateEvents(tmva_reader,vec_variables,vec_variablesSpec,string(conf.GetClassifierOpt()),method);
  //Evaluate TMVA-----------------------------------
}
