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

using namespace std;

map<int,shared_ptr<EvalTMVA>> evaluater;

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
  map<int,string> mva_loc;
  string conf_loc;
  if (mva_index==0) {
    mva_loc[1] = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_0/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_1/factory_BDT_1_Score.weights.xml";
    conf_loc = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/config_SM4top_bdt.conf";
  }
  else if (mva_index==1) {
    mva_loc[1] = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    conf_loc = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf";
  }
  else if (mva_index==2) {
    mva_loc[1] = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    conf_loc = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf";
  }
  else if (mva_index==3) {
    mva_loc[91] = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    mva_loc[101] = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    conf_loc = "/afs/cern.ch/work/a/asonay/public/config_bdt_crossvalidation_pflow.conf";
  }
  else {
    cout << "NO Valid option defined!"
	 << "\n0 = SM4top"
	 << "\n1 = Inclusive Standard BDT"
	 << "\n2 = Inclusive Standard BDT+Q/G"
	 << "\n3 = Exclusive Standard BDT+Q/G" << endl;
    mva_loc[1] = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_0/factory_BDT_0_Score.weights.xml,/afs/cern.ch/work/a/asonay/public/SM4top_BDT/loader_1/factory_BDT_1_Score.weights.xml";
    conf_loc = "/afs/cern.ch/work/a/asonay/public/SM4top_BDT/config_SM4top_bdt.conf";
  }

  for (auto const &x : mva_loc)
    {evaluater[x.first] = shared_ptr<EvalTMVA>(new EvalTMVA(x.second,conf_loc));}
  
  for (auto const &x : evaluater){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }
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
