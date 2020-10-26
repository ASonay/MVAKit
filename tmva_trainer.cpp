
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

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "inc/TMVAConf.hpp"
//#include "inc/ReadTree.hpp"


using namespace std;

int main(int argc,char **argv)
{
  // Configuration part-----------------------------
  TMVAConf conf("conf");
    
  conf.Parser(argc,argv);
  conf.ReadConf();
  
  vector<string> split = conf.GetSplit();
  vector<TString> variables = conf.GetTVariables();
  vector<TString> variablesSpec = conf.GetTVariablesOther();
  // Configuration part-----------------------------
  // Reading part-----------------------------------
  vector<TMVA::DataLoader*> loaders;
  
  for (unsigned i=0;i<split.size();i++){
    TString loader_name = "loader_"+to_string(i);
    TMVA::DataLoader* loader = new TMVA::DataLoader(loader_name);
    for (auto var : variables) loader->AddVariable(var);
    for (auto var : variablesSpec) loader->AddSpectator(var);
    loaders.push_back(loader);
  }
  conf.SetEvents(loaders);
  // Reading part-----------------------------------
  // Training part----------------------------------
  for (unsigned i=0;i<split.size();i++){
    loaders.at(i)->PrepareTrainingAndTestTree(TCut(""),conf.GetSamplingOpt());
    TString factory_name = "factory_"+conf.GetClassifierOpt()+"_"+to_string(i);
    TFile *outputFile = TFile::Open( factory_name+".root", "RECREATE" );
    TMVA::Factory *factory = new TMVA::Factory(factory_name , outputFile, conf.GetFactoryOpt() );
    TString title = "Score";
    factory->BookMethod(loaders.at(i),conf.GetClassifierOpt(),title,conf.GetTrainingOpt());
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    
    outputFile->Close();
  }
  // Training part----------------------------------
}
