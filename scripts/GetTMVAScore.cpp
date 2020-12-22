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

map<int,pair<string,string>> training = {{0,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {1,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhHerwig/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {2,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_aMcAtNloPy8/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_hf.conf"}},
					 {3,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 {4,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhHerwig/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 {5,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_aMcAtNloPy8/os2l/kin/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_os2l_kin.conf"}},
					 {6,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/ttbar_PhPy8/ljets/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_ljets_hf.conf"}},
					 {99,{"/afs/cern.ch/work/a/asonay/public/PNN_reweighting/multiclass/os2l/hf/loader_0/weights/factory_PyKeras_0_Score.weights.xml","/config/config_pnnrw_multiclass_hf.conf"}}
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
