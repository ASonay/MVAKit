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
#include <stdarg.h>
#include <unistd.h>

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
#include "TMVA/PyMethodBase.h"
#include "TMVA/Tools.h"

#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "MVAKit/EvalTMVA.hpp"
#include "MVAKit/NeuralNetwork.hpp"
#include "MVAKit/Transformers.hpp"

using namespace std;

vector<unique_ptr<NeuralNetwork>> nn;
vector<unique_ptr<Transformers>> tr;

map<int,unique_ptr<EvalTMVA>> evaluater;

void GetTMVAScore(int mH=400,int mva_index=0)
{
  string mvakit_dir = getenv("MVAKIT_HOME");
  string lib = mvakit_dir + "/build/lib/libMVAKit.so";
  if (gSystem->Load(lib.c_str())==0)
    {cout << "Your library, " << lib << " successfully loaded." << endl;}
  else if (gSystem->Load(lib.c_str())==1)
    {cout << "Your library, " << lib << " successfully loaded before." << endl;}
  else{
    cout << "Your library, " << lib << " cannot be loaded." << endl;
    exit(0);
  }
  map<int,pair<string,string>> file_loc;
  if (mva_index==0) {
    file_loc[1].first = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_0/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_1/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/config_SM4top_bdt.conf";
  }
  else if (mva_index==1) {
    file_loc[91].first = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/9jge3b/loader_0/weights/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/9jge3b/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[101].first = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/ge10jge3b/loader_0/weights/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/ge10jge3b/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[91].second = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/9jge3b/config_SM4top_bdt.conf";
    file_loc[101].second = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/exclusive/ge10jge3b/config_SM4top_bdt.conf";
  }
  else if (mva_index==2) {
    file_loc[1].first = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf";
  }
  else if (mva_index==3) {
    file_loc[1].first = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf";
  }
  else if (mva_index==21) {
    file_loc[91].first = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[101].first = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/ge10jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/ge10jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[91].second = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/9jge3b/config_bdt_BSM4t_crossvalidation.conf";
    file_loc[101].second = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/BSM4tVar/config_bdt_BSM4t_crossvalidation.conf";
  }
  else if (mva_index==4) {
    file_loc[91].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[101].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[91].second = "/afs/cern.ch/work/a/asonay/public/config_bdt_crossvalidation_pflow.conf";
    file_loc[101].second = "/afs/cern.ch/work/a/asonay/public/config_bdt_crossvalidation_pflow.conf";
  }
  else if (mva_index==5) {
    file_loc[1].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/afs/cern.ch/work/a/asonay/public/config_bdt_crossvalidation.conf";
    file_loc[2].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[2].second = "/afs/cern.ch/work/a/asonay/public/config_bdt_crossvalidation_2l.conf";
  }
  else {
    cout << "NO Valid option defined!"
	 << "\n0 = SM4top"
	 << "\n1 = SM4top Exclusive BDT"
	 << "\n2 = Inclusive Standard BDT"
	 << "\n3 = Inclusive Standard BDT+Q/G"
	 << "\n4 = Exclusive Standard BDT+Q/G" << endl;
    file_loc[1].first = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_0/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_1/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/config_SM4top_bdt.conf";
  }

  for (auto const &x : file_loc)
    {evaluater[x.first] = unique_ptr<EvalTMVA>(new EvalTMVA(x.second.first,x.second.second));}
  
  for (auto const &x : evaluater){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }

  nn.emplace_back(new NeuralNetwork("/afs/cern.ch/work/a/asonay/reweighting/ljets_nominal/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/keras_output/model/model_0.txt", "/afs/cern.ch/work/a/asonay/TMVA_tool/config/config_nnrew_ljets_kin.conf"));
  tr.emplace_back(new Transformers("std", "/afs/cern.ch/work/a/asonay/reweighting/ljets_nominal/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/keras_output/feature_weight/fold0_std_scaler.txt"));
}

template<typename... Args>
Double_t GetNNScore(int index, Args... args){
  vector<float> x = { float{args}... };

  tr[index]->StdTransform(x);
  nn[index]->SetInputs(x);
  nn[index]->FeedForward();
  
  return nn[index]->GetOutput();
}

template<typename... Args>
Double_t GetNNRew(int index, int lep, Args... args){
  vector<float> x = { float{args}... };

  tr[index]->StdTransform(x);
  nn[index]->SetInputs(x);
  nn[index]->FeedForward();

  float score = nn[index]->GetOutput();

  if (lep==1) {return score/(1.0-score);}
  else {return exp(score);}
}

template<typename... Args>
Double_t GetScore(int ev, long long en, Args... args){
  vector<Double_t> x = { float{args}... };
  int index = ev;
  return evaluater[index]->GetScore(x,en);
}

template<typename... Args>
Double_t GetScoreEx(int ev, int njet, long long en, Args... args){
  vector<Double_t> x = { float{args}... };
  int index = njet==9 ? 90 : 100;
  index+=ev;
  return evaluater[index]->GetScore(x,en);
}
