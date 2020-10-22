
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
  vector<string> ntupsSig,ntupsBkg;
  string file;
  
  TMVAConf conf;
    
  conf.Parser(argc,argv,ntupsSig,ntupsBkg,file);
  
  cout << "Configuration File: " << file << endl;
  
  vector<string> variables = {"HT_all","nJets"};
  vector<string> split ={"eventNumber%2==0"};
  // Configuration part-----------------------------
  // Reading part-----------------------------------

  vector<TMVA::DataLoader*> loaders;

  conf.SetConf(ntupsSig,ntupsBkg,variables,split,"1","1","nBTags_MV2c10_70>=3","nBTags_MV2c10_70>=3");

  for (unsigned i=0;i<split.size();i++){
    TString loader_name = "loader_"+to_string(i);
    TMVA::DataLoader* loader = new TMVA::DataLoader(loader_name);
    for (auto var : variables) loader->AddVariable(var);
    loaders.push_back(loader);
  }

  conf.SetEvents(loaders);

  // Reading part-----------------------------------
  // Training part----------------------------------
  for (unsigned i=0;i<split.size();i++){
    loaders.at(i)->PrepareTrainingAndTestTree(TCut(""),"nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    TString factory_name = "factory_"+to_string(i);
    TFile *outputFile = TFile::Open( factory_name+".root", "RECREATE" );
    TMVA::Factory *factory = new TMVA::Factory(factory_name , outputFile, "!V:!Silent:Transformations=I;D;P;G,D:AnalysisType=Classification" );
    factory->BookMethod(loaders.at(i),TMVA::Types::kBDT,"BDTG","!H:!V:NTrees=600:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.6:SeparationType=GiniIndex:nCuts=30:MaxDepth=2:NegWeightTreatment=IgnoreNegWeightsInTraining");
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    
    outputFile->Close();
  }
  // Training part----------------------------------

  return 0;
}
