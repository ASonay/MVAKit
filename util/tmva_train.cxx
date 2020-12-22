
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
#include "TMVA/Tools.h"
#include "TMVA/PyMethodBase.h"


#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "MVAKit/MVAKit.hpp"

using namespace std;

int main(int argc,char **argv)
{
  // Configuration part-----------------------------
  auto tool = make_unique<MVAKit>("Training");
    
  tool->Parser(argc,argv);
  tool->ReadConf();
  if (Common::StringCompare(string(tool->GetClassifierOpt()),"py")){
    TMVA::Tools::Instance();
    TMVA::PyMethodBase::PyInitialize();
    tool->ExecuteModel();
  }

  vector<string> split = tool->GetSplit();
  vector<TString> variables = tool->GetTVariables();
  vector<TString> variablesSpec = tool->GetTVariablesOther();
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
  tool->SetLoaders(loaders);
  tool->SetEvents();
  // Reading part-----------------------------------
  // Training part----------------------------------
  for (unsigned i=0;i<split.size();i++){
    loaders[i]->PrepareTrainingAndTestTree(TCut(""),tool->GetSamplingOpt());
    TString factory_name = "factory_"+tool->GetClassifierOpt()+"_"+to_string(i);
    TFile *outputFile = TFile::Open( factory_name+".root", "RECREATE" );
    TMVA::Factory *factory = new TMVA::Factory(factory_name , outputFile, tool->GetFactoryOpt() );
    TString title = "Score";
    factory->BookMethod(loaders.at(i),TMVA::Types::kPyKeras,title,tool->GetTrainingOpt());
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    
    outputFile->Close();
  }
  // Training part----------------------------------
}
