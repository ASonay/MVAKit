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

//REWEIGHTING-----------------------------
vector<unique_ptr<NeuralNetwork>> nn;
vector<unique_ptr<Transformers>> tr;
vector<pair<string,string>> nn_config;

//BDT-------------------------------------
map<int,unique_ptr<EvalTMVA>> evaluater;

void GetTMVAScore(int mH=400,int mva_index=0,int input_dir_index=0)
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

  //REWEIGHTING-----------------------------
  //string input_dir_reweighting="/eos/user/b/bsm4tops/bsm4teos/reweighting/";
  string input_dir_reweighting="/eos/user/b/bsm4tops/bsm4teos/reweighting/v212169/";
  if (input_dir_index==1) input_dir_reweighting="/local_scratch/sched3am/BSM4t/reweighting/";
  nn_config = {
  //0 ttbar 1L nominal
  {input_dir_reweighting+"ljets_nominal/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"ljets_nominal/config_nnrew_ljets_kin.conf"},
  //1 ttbar 1L nominal up 50% unc. to reweighted ttbar
  {input_dir_reweighting+"../ljets_nominal/up_var/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/",input_dir_reweighting+"../ljets_nominal/up_var/config_nnrew_ljets_kin.conf"},
  //2 ttbar 1L nominal down 50% unc. to reweighted ttbar
  {input_dir_reweighting+"../ljets_nominal/down_var/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/",input_dir_reweighting+"../ljets_nominal/down_var/config_nnrew_ljets_kin.conf"},
  //3 ttbar 1L PhHw
  {input_dir_reweighting+"../ljets_PhHw/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/",input_dir_reweighting+"../ljets_PhHw/config_nnrew_alt_ljets_kin.conf"},
  //4 ttbar 1L Mg5Py8
  {input_dir_reweighting+"../ljets_Mg5Py8/pnnrewLRate_0_0036_Momentum_0_9_Epoch_120_Batch_1000_Optimizer_SGD_Loss_mean_squared_error/",input_dir_reweighting+"../ljets_Mg5Py8/config_nnrew_alt_ljets_kin.conf"},
  //5 ttbar 1L ttbb
  {input_dir_reweighting+"ljets_ttbb/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"ljets_ttbb/config_nnrew_ljets_kin.conf"},
 
  //6 ttbar 2LOS nominal
  {input_dir_reweighting+"os2l_nominal/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"os2l_nominal/config_nnrew_os2l_kin.conf"},
  //7 ttbar 2LOS nominal up 50% unc. to reweighted ttbar
  {input_dir_reweighting+"../os2l_nominal/up_var/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"../os2l_nominal/up_var/config_nnrew_os2l_kin.conf"},
  //8 ttbar 2LOS nominal down 50% unc. to reweighted ttbar
  {input_dir_reweighting+"../os2l_nominal/down_var/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"../os2l_nominal/down_var/config_nnrew_os2l_kin.conf"},
  //9 ttbar 2LOS PhHw
  {input_dir_reweighting+"os2l_PhHw/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"os2l_PhHw/config_nnrew_os2l_kin.conf"},
  //10 ttbar 2LOS Mg5Py8
  {input_dir_reweighting+"os2l_Mg5Py8/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"os2l_Mg5Py8/config_nnrew_os2l_kin.conf"},
  //11 ttbar 2LOS ttbb
  {input_dir_reweighting+"os2l_ttbb/pnnrewLRate_0_05_Momentum_0_9_Epoch_320_Batch_1000_Optimizer_SGD_Loss_Exp_LRate_dec_0_9_LRate_step_10000/",input_dir_reweighting+"os2l_ttbb/config_nnrew_os2l_kin.conf"},
};



  //BDT-------------------------------------  
  string input_dir_1L = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/1L/";
  string input_dir_2LOS = "/eos/user/k/ktam/public/Htt/BDT/PFlow_Setup/training/2LOS/";
  if (input_dir_index==1){
    input_dir_1L = "/local_scratch/sched3am/BDT_terry/1L/";
    input_dir_2LOS = "/local_scratch/sched3am/BDT_terry/2LOS/";
  }
  map<int,pair<string,string>> file_loc;
  if (mva_index==0) {
    file_loc[1].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/config_SM4top_bdt.conf";
    file_loc[2].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/os2l/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/os2l/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[2].second = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/os2l/config_SM4top_bdt.conf";
  }
  else if (mva_index==1) {
    file_loc[91].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/9jge3b/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/9jge3b/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[101].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/ge10jge3b/loader_0/weights/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/ge10jge3b/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[91].second = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/9jge3b/config_SM4top_bdt.conf";
    file_loc[101].second = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/exclusive/ge10jge3b/config_SM4top_bdt.conf";
  }
  else if (mva_index==2) {
    file_loc[1].first = input_dir_1L+"BSM4tVar/ge9jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,"+input_dir_1L+"BSM4tVar/ge9jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = input_dir_1L+"BSM4tVar/config_bdt_BSM4t_crossvalidation.conf";

    file_loc[2].first = input_dir_2LOS+"BSM4tVar/ge7jge3b/M"+to_string(mH)+"/loader_0/weights/factory_BDT_0_Score.weights.xml,"+input_dir_2LOS+"BSM4tVar/ge7jge3b/M"+to_string(mH)+"/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[2].second = input_dir_2LOS+"BSM4tVar/config_bdt_BSM4t_crossvalidation_2l.conf";
  }
  else if (mva_index==3) {
  	file_loc[1].first = input_dir_1L+"SM4tVar/ge9jge3b/SM4t/loader_0/weights/factory_BDT_0_Score.weights.xml,"+input_dir_1L+"SM4tVar/ge9jge3b/SM4t/loader_1/weights/factory_BDT_1_Score.weights.xml";
  	file_loc[1].second = input_dir_1L+"SM4tVar/config_bdt_SM4t_crossvalidation.conf";

    file_loc[2].first = input_dir_2LOS+"SM4tVar/ge7jge3b/SM4t/loader_0/weights/factory_BDT_0_Score.weights.xml,"+input_dir_2LOS+"SM4tVar/ge7jge3b/SM4t/loader_1/weights/factory_BDT_1_Score.weights.xml";
    file_loc[2].second = input_dir_2LOS+"SM4tVar/config_bdt_SM4t_crossvalidation_2l.conf";
  }
  else if (mva_index==-1) {
    cout << "No BDT option choosen" << endl;
  }
  else {
    cout << "NO Valid option defined!"
	 << "\n0 = SM4top"
	 << "\n1 = SM4top Exclusive BDT"
	 << "\n2 = Inclusive Standard BDT" << endl;
    file_loc[1].first = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/loader_0/factory_BDT_0_Score.weights.xml,/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/loader_1/factory_BDT_1_Score.weights.xml";
    file_loc[1].second = "/eos/user/b/bsm4tops/bsm4teos/MVA_study_1LOS/SM4top_BDT/ljets/config_SM4top_bdt.conf";
  }
  if (mva_index!=-1) {
    for (auto const &x : file_loc)
      {evaluater[x.first] = unique_ptr<EvalTMVA>(new EvalTMVA(x.second.first,x.second.second));}
  
    for (auto const &x : evaluater){
      x.second->Init();
      cout << x.second->GetScore({1}) << endl;;
    }
  }
  //BDT-------------------------------------

  //REWEIGHTING-----------------------------
  for (auto x : nn_config){
    tr.emplace_back(new Transformers("std", x.first+"keras_output/feature_weight/fold0_std_scaler.txt"));
    nn.emplace_back(new NeuralNetwork());
    nn.back()->SetInputs(tr.back()->TransformedData());
    nn.back()->CreateNetwork(x.first+"keras_output/model/model_0.txt", x.second);
  }
  //REWEIGHTING-----------------------------
}

template<typename... Args>
Double_t GetNNScore(int index, Args... args){
  vector<float> x = { float{args}... };

  tr[index]->StdTransform(x);
  nn[index]->FeedForward();
  
  return nn[index]->GetOutput();
}

template<typename... Args>
Double_t GetNNRew(int mc, int lep, Args... args){
  vector<float> x = { float{args}... };

  int sample = 0;

  if (mc==407344||mc==407343||mc==407342||mc==411075||mc==411078||mc==411076||mc==411077||mc==411074||mc==411073||mc==410472||mc==410470) {sample=0;}
  else if (mc==410557||mc==410558||mc==411083||mc==411086||mc==411084||mc==411085||mc==411087||mc==411082||mc==407355||mc==407354||mc==407356) {sample=3;}
  else if (mc==410465||mc==410464||mc==412068||mc==412066||mc==412067||mc==412069||mc==412070||mc==412071||mc==407348||mc==407349||mc==407350) {sample=4;}
  else if (mc==600791||mc==600792) {sample=5;}
  else {cout << "Your DSID: " << mc << " does not match any of ttbar sample." << endl; exit(0);}
  
  int index = lep==1?sample:sample+6;
  
  tr[index]->StdTransform(x);
  nn[index]->FeedForward();

  float score = nn[index]->GetOutput();
  
  //cout << "Running " << mc << " sample " << sample << " index " << index << " lep number " << lep << " score " << score << endl;

  return exp(score);
}

template<typename... Args>
Double_t GetNNRewUp(int lep, Args... args){
  vector<float> x = { float{args}... };

  int index1 = lep==1?0:5;
  int index2 = lep==1?1:6;

  tr[index1]->StdTransform(x);
  nn[index1]->FeedForward();
  tr[index2]->StdTransform(x);
  nn[index2]->FeedForward();

  float score1 = nn[index1]->GetOutput();
  float rew1 = lep==1?score1/(1.0-score1):exp(score1);
  float score2 = nn[index2]->GetOutput();
  float rew2 = lep==1?score2/(1.0-score2):exp(score2);

  return (rew1+abs(rew1-1.5*rew2));
}

template<typename... Args>
Double_t GetNNRewDown(int lep, Args... args){
  vector<float> x = { float{args}... };

  int index1 = lep==1?0:5;
  int index2 = lep==1?2:7;

  tr[index1]->StdTransform(x);
  nn[index1]->FeedForward();
  tr[index2]->StdTransform(x);
  nn[index2]->FeedForward();

  float score1 = nn[index1]->GetOutput();
  float rew1 = lep==1?score1/(1.0-score1):exp(score1);
  float score2 = nn[index2]->GetOutput();
  float rew2 = lep==1?score2/(1.0-score2):exp(score2);

  return (rew1-abs(rew1-0.5*rew2));
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
