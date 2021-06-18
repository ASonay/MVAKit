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

using namespace std;

bool isInit = false;
int map_index=-100;
bool check = false;

map<int,pair<string,string>> training = {					 
					 //BDT
					//  {4001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {5001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {6001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {7001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {8001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {9001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {10001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/ljets/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation.conf"}},
					//  {4002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {5002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {6002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {7002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {8002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {9002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
					//  {10002,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights/non_parameterized/os2l/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_2l.conf"}},
           // new preliminary training ljet regions: without the qg tagger
					 {4003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M400/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {5003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M500/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {6003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M600/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {7003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M700/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {8003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M800/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {9003, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M900/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
					 {10003,{"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/ge9jge3b/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet1/config_bdt_crossvalidation.conf"}},
           // new preliminary training ljet regions: with the qg tagger and eta cut
					 {4004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M400/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {5004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M500/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {6004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M600/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {7004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M700/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {8004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M800/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {9004, {"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M900/loader_1/weights/factory_BDT_1_Score.weights.xml"  ,"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},
					 {10004,{"/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/ge9jge3b/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/VarSet6_QGTagging_eta/config_bdt_crossvalidation.conf"}},

};

map<int,pair<string,string>> training_pflow_9j3b = {					 
					 //BD/T
					 {4001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {5001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {6001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {7001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {8001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {9001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {10001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_9jge3b/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}}
};

map<int,pair<string,string>> training_pflow_10j3b = {					 
					 //BD/T
					 {4001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M400/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M400/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {5001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M500/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M500/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {6001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M600/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M600/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {7001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M700/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M700/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {8001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M800/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M800/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {9001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M900/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M900/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}},
					 {10001,{"/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M1000/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/BDT_weights_212120_v2/ljets_ge10jge3b/M1000/loader_1/weights/factory_BDT_1_Score.weights.xml","/config/config_bdt_crossvalidation_pflow.conf"}}
};

map<int,shared_ptr<EvalTMVA>> evaluater;
map<int,shared_ptr<EvalTMVA>> evaluater_pflow_9j3b;
map<int,shared_ptr<EvalTMVA>> evaluater_pflow_10j3b;

void GetTMVAScore(int ev=-100)
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

  for (auto const &x : training){
    auto confFile = mvakit_dir+x.second.second;
    if(access( x.second.second.c_str(), F_OK ) != -1) confFile = x.second.second; // if x.second.second is a valid path, use it directly
    cout << mvakit_dir+x.second.second << endl;evaluater[x.first] = shared_ptr<EvalTMVA>(new EvalTMVA(x.second.first,confFile));
  }

  for (auto const &x : evaluater){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }

  map_index=ev;
  isInit=true;
}

void GetTMVAScorePFlow(int ev=-100)
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

  for (auto const &x : training_pflow_9j3b)
    {cout << mvakit_dir+x.second.second << endl;evaluater_pflow_9j3b[x.first] = shared_ptr<EvalTMVA>(new EvalTMVA(x.second.first,mvakit_dir+x.second.second));}

  for (auto const &x : training_pflow_10j3b)
    {cout << mvakit_dir+x.second.second << endl;evaluater_pflow_10j3b[x.first] = shared_ptr<EvalTMVA>(new EvalTMVA(x.second.first,mvakit_dir+x.second.second));}

  for (auto const &x : evaluater_pflow_9j3b){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }
  for (auto const &x : evaluater_pflow_10j3b){
    x.second->Init();
    cout << x.second->GetScoreRatio({1}) << endl;;
  }

  map_index=ev;
  isInit=true;
}

void MassPoint(int mp){
  map_index = mp;
  check = true;
  cout << "Mapping changed by: " << map_index << endl;
}

template<typename... Args>
Double_t GetVal(int ev, long long en, int cl1, int cl2, Args... args){
  
  if (!isInit) GetTMVAScore();
  
  vector<Double_t> x = { float{args}... };

  return evaluater[ev]->GetScoreRatio(x,en,cl1,cl2);
}

template<typename... Args>
Double_t GetScore(int ev, long long en, Args... args){

  if (!isInit) GetTMVAScore();
  
  vector<Double_t> x = { float{args}... };

  int index = map_index==-100 ? ev : map_index*10+ev;

  if (check){
    cout << index << endl;
    check=false;
  }
  
  return evaluater[index]->GetScore(x,en);
}

template<typename... Args>
Double_t GetScorePFlow(int ev, long long en, Args... args){

  if (!isInit) GetTMVAScorePFlow();
  
  vector<Double_t> x = { float{args}... };

  int index = map_index==-100 ? ev : map_index*10+ev;

  if (check){
    cout << index << endl;
    check=false;
  }

  if (x[2]==9)
    {return evaluater_pflow_9j3b[index]->GetScore(x,en);}
  else
    {return evaluater_pflow_10j3b[index]->GetScore(x,en);}
}
