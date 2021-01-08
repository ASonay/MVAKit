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

bool isInit = false;

map<int,pair<string,string>> training = {
					 //REWEIGHTING
					 {0,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {1,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhHerwig/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {2,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_aMcAtNloPy8/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {3,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 {4,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhHerwig/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 {5,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_aMcAtNloPy8/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 //{6,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/ljets/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_ljets_hf.conf"}}
					 //BDT
					 {4001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {5001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {6001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {7001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {8001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {9001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {10001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					 {4002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {5002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {6002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {7002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {8002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {9002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					 {10002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}}
};

map<int,unique_ptr<EvalTMVA>> evaluater;

void GetTMVAScore()
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

  for (auto const &x : training)
    {evaluater[x.first] = unique_ptr<EvalTMVA>(new EvalTMVA(x.second.first,mvakit_dir+x.second.second));}

  for (auto const &x : evaluater){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }
  
  isInit=true;
}

template<typename... Args>
Double_t GetVal(int ev, int en, int cl1, int cl2, Args... args){
  
  if (!isInit) GetTMVAScore();
  
  vector<Double_t> x = { float{args}... };

  return evaluater[ev]->GetScoreRatio(x,en,cl1,cl2);
}

template<typename... Args>
Double_t GetScore(int ev, int en, Args... args){

  if (!isInit) GetTMVAScore();
  
  vector<Double_t> x = { float{args}... };

  return evaluater[ev]->GetScore(x,en);
}
