#include "helper.cpp"


void validate_classifier()
{
  
  string tmva_path = "../../bsm4top_bdt_weight_cv/ljets/xs/";
  string original_file_path = "/eos/user/a/asonay/HBSM4top_ntuple/";
  string config_file = "config_massparam_bdt_crossvalidation.conf";
  string app_path = "../";
  
  string class_name = "Score";
  string cut = "(nJets>=9&&nBTags_MV2c10_70>=3)";

  string xml_files = tmva_path+"loader_0/weights/factory_BDT_0_Score.weights.xml,"+tmva_path+"loader_1/weights/factory_BDT_1_Score.weights.xml";
  string init_exp = "Init(\""+app_path+"\",\""+xml_files+"\",\""+app_path+"config/config_massparam_bdt_crossvalidation.conf\")";
  
  gROOT->ProcessLine((".include "+app_path).c_str());
  gROOT->ProcessLine(".L GetTMVAScore.cpp+");
  gROOT->ProcessLine(init_exp.c_str());
  
  
  TFile *f1 = new TFile((tmva_path+"factory_BDT_0.root").c_str());
  TFile *f2 = new TFile((tmva_path+"factory_BDT_1.root").c_str());
  TTree *t_train_1 = (TTree*)f1->Get("loader_0/TrainTree");
  TTree *t_test_1 = (TTree*)f1->Get("loader_0/TestTree");
  TTree *t_train_2 = (TTree*)f2->Get("loader_1/TrainTree");
  TTree *t_test_2 = (TTree*)f2->Get("loader_1/TestTree");

  vector<string> MP = {"400","500","600","700","800","900","1000"};

  vector<double> mass = {400,500,600,700,800,900,1000};
  vector<double> v_roc_train;
  vector<double> v_roc_test;
  vector<double> v_roc_cv;
  vector<double> v_roc_sm;
  vector<double> v_roc_in;
  vector<double> v_sep_train;
  vector<double> v_sep_test;
  vector<double> v_sep_cv;
  vector<double> v_sep_sm;
  vector<double> v_sep_in;
    
  vector<string> suffix = {"ljets9j","ljetsge10j"};
  vector<string> mc_samp = {"mc16a","mc16d","mc16e"};

  TChain *chain_bkg = new TChain("nominal_Loose");
  for (auto suf : suffix){
    for (auto mc : mc_samp){
      string fullname_bkg = original_file_path+suf+"/"+"ttbar_PhPy8_AFII_AllFilt_"+mc+".root";
      cout << fullname_bkg << endl;
      chain_bkg->Add(fullname_bkg.c_str());
    }
  }
  TTree *tr_bkg = chain_bkg;

  TChain *chain_sig_sm = new TChain("nominal_Loose");
  for (auto suf : suffix){
    for (auto mc : mc_samp){
      string fullname_sig = "/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/"+suf+"/"+"tttt_aMcAtNloPy8_"+mc+".root";
      cout << fullname_sig << endl;
      chain_sig_sm->Add(fullname_sig.c_str());
    }
  }
  TTree *tr_sig_sm = chain_sig_sm;

  TChain *chain_bkg_sm = new TChain("nominal_Loose");
  for (auto suf : suffix){
    for (auto mc : mc_samp){
      string fullname_bkg = "/eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/"+suf+"/"+"ttbar_PhPy8_AFII_AllFilt_"+mc+".root";
      cout << fullname_bkg << endl;
      chain_bkg_sm->Add(fullname_bkg.c_str());
    }
  }
  TTree *tr_bkg_sm = chain_bkg_sm;
  
  TH1F *hSig_sm = getHist(tr_sig_sm,"hSig_sm","BDT_1l_score",cut+"*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");

  TH1F *hBkg_sm = getHist(tr_bkg_sm,"hBkg_sm","BDT_1l_score",cut+"*(MyWeight_nJets_nRCJets(nRCJetsM100,nJets,nElectrons+nMuons,nRCJetsM100,0)*MyWeight_HT_all_red((HT_all/1.e3)-((nJets-7)*90*((nElectrons+nMuons)==1))-((nJets-5)*90*((nElectrons+nMuons)==2)),nJets,nElectrons+nMuons,nRCJetsM100,0)*MyWeight_dRjj_Avg(dRjj_Avg,nJets,nElectrons+nMuons,nRCJetsM100,0))*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");
    
  TMVA::ROCCalc calc_sm(hSig_sm,hBkg_sm);
  double roc_sm = calc_sm.GetROCIntegral();
  calc_sm.ResetStatus();

  cout << "SM 4top BDT ROC: " << roc_sm << endl;
  double sep_sm = GetSeparation(hSig_sm,hBkg_sm);
  cout << "SM 4top BDT SEP: " << sep_sm << endl;

  for (auto mp : MP){

    TChain *chain_sig = new TChain("nominal_Loose");
    for (auto suf : suffix){
      for (auto mc : mc_samp){
	string fullname_sig = original_file_path+suf+"/"+"ttH_tttt_m"+mp+"_"+mc+".root";
	cout << fullname_sig << endl;
	chain_sig->Add(fullname_sig.c_str());
      }
    }
    TTree * tr_sig = chain_sig;

    string mva_var = "GetScore(eventNumber,HT_all,jet_pt[0],nJets,Centrality_all,dRjj_Avg,dRbb_MindR_MV2c10_70,dRbl_MindR_MV2c10_70,Mjjj_MindR,Mbbb_Avg_MV2c10_70,Mbb_MinM_MV2c10_70,nRCJetsM100,Sum$(rcjet_d12),Sum$(rcjet_d23),mtw,met_met,Sum__jet_pcb_MV2c10_btag_ordered_T__Iteration__6__,"+mp+")";
    
    TH1F *hSig_cv = getHist(tr_sig,"hSig_cv",mva_var,cut+"*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");

    TH1F *hBkg_cv = getHist(tr_bkg,"hBkg_cv",mva_var,cut+"*((NN_score_hf/(1.-NN_score_hf))*(NN_score_kin/(1.-NN_score_kin)))*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");

    TH1F *hSigsm = getHist(tr_sig,"hSigsm","BDT_1l_score",cut+"*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");
    
    string cut_exp_bkg = "(classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
    TH1F *hBkg_test = getHist(t_test_1,"htestBkg",class_name.c_str(),cut_exp_bkg);
    hBkg_test->Add(getHist(t_test_2,"htestBkg2",class_name.c_str(),cut_exp_bkg));
    TH1F *hBkg_train = getHist(t_train_1,"htrainBkg",class_name.c_str(),cut_exp_bkg);
    hBkg_train->Add(getHist(t_train_2,"htrainBkg2",class_name.c_str(),cut_exp_bkg));
    string cut_exp_sig = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
    TH1F *hSig_test = getHist(t_test_1,"htestSig"+mp,class_name.c_str(),cut_exp_sig);
    hSig_test->Add(getHist(t_test_2,"htestSig2"+mp,class_name.c_str(),cut_exp_sig));
    TH1F *hSig_train = getHist(t_train_1,"htrainSig"+mp,class_name.c_str(),cut_exp_sig);
    hSig_train->Add(getHist(t_train_2,"htrainSig2"+mp,class_name.c_str(),cut_exp_sig));
    draw_test_train_val("BDT-Score-M"+mp,cut,hSig_train,hBkg_train,hSig_test,hBkg_test,hSig_cv,hBkg_cv);

    TMVA::ROCCalc calc_train(hSig_train,hBkg_train);
    double roc_train = calc_train.GetROCIntegral();
    calc_train.ResetStatus();
    TMVA::ROCCalc calc_test(hSig_test,hBkg_test);
    double roc_test = calc_test.GetROCIntegral();
    calc_test.ResetStatus();
    TMVA::ROCCalc calc_cv(hSig_cv,hBkg_cv);
    double roc_cv = calc_cv.GetROCIntegral();
    calc_cv.ResetStatus();
    TMVA::ROCCalc calcsm(hSigsm,hBkg_sm);
    double rocsm = calcsm.GetROCIntegral();
    calcsm.ResetStatus();

    cout << "Param: " << mp << " ROC Train: " << roc_train << " ROC Test: " << roc_test << " ROC CV: " << roc_cv << endl;

    double sep_train = GetSeparation(hSig_train,hBkg_train);
    double sep_test = GetSeparation(hSig_test,hBkg_test);
    double sep_cv = GetSeparation(hSig_cv,hBkg_cv);
    double sepsm = GetSeparation(hSigsm,hBkg_sm);
    cout << " SEP Train: " << sep_train << " SEP Test: " << sep_test << endl << " SEP Cv: " << sep_cv << endl;

    v_roc_train.push_back(roc_train);v_roc_test.push_back(roc_test);v_roc_cv.push_back(roc_cv);v_roc_sm.push_back(rocsm);
    v_sep_train.push_back(sep_train);v_sep_test.push_back(sep_test);v_sep_cv.push_back(sep_cv);v_sep_sm.push_back(sepsm);

  }

  TCanvas *croc = new TCanvas("croc");
  croc->SetTickx(); croc->SetTicky();
  
  TGraph *groctrain = new TGraph((int)mass.size(),&mass[0],&v_roc_train[0]);
  TGraph *groctest = new TGraph((int)mass.size(),&mass[0],&v_roc_test[0]);
  TGraph *groccv = new TGraph((int)mass.size(),&mass[0],&v_roc_cv[0]);
  TGraph *grocsm = new TGraph((int)mass.size(),&mass[0],&v_roc_sm[0]);
  groctrain->SetLineColor(kAzure-3);groctrain->SetLineWidth(4);
  groctrain->SetMarkerColor(kAzure-3);groctrain->SetMarkerStyle(20);
  groctest->SetLineColor(kOrange+1);groctest->SetLineWidth(4);
  groctest->SetMarkerColor(kOrange+1);groctest->SetMarkerStyle(25);
  groccv->SetLineColor(kRed-4);groccv->SetLineWidth(4);groccv->SetLineStyle(2);
  groccv->SetMarkerColor(kRed-4);groccv->SetMarkerStyle(5);
  grocsm->SetLineColor(kGray+3);grocsm->SetLineWidth(3);grocsm->SetLineStyle(9);
  grocsm->SetMarkerColor(kGray+3);grocsm->SetMarkerStyle(22);
  
  double max_roc = *max_element(v_roc_cv.begin(), v_roc_cv.end());
  double min_roc = *min_element(v_roc_cv.begin(), v_roc_cv.end());

  if (roc_sm<min_roc) min_roc=roc_sm;
  
  TH2F *hcan_roc = new TH2F("hcan_roc","",100,300,1100,100,0.98*min_roc,1.02*max_roc);
  hcan_roc->GetXaxis()->SetTitle("Mass");
  hcan_roc->GetYaxis()->SetTitle("AUC");
  hcan_roc->Draw("same");

  groccv->Draw("lp same");
  groctrain->Draw("lp same");
  groctest->Draw("lp same");
  grocsm->Draw("lp same");
  
  TLegend *l_roc = new TLegend(0.140375,0.746269,0.457226,0.922886);
  l_roc->SetLineWidth(0);
  l_roc->SetFillStyle(0);
  l_roc->AddEntry(groctrain,"Train","lp");
  l_roc->AddEntry(groctest,"Test","lp");
  l_roc->AddEntry(groccv,"Cross-validation","lp");
  l_roc->AddEntry(grocsm,"SM 4top","lp");
  l_roc->Draw();

  TLine *line_roc = new TLine(400,roc_sm,1000,roc_sm);
  line_roc->SetLineWidth(3);line_roc->SetLineColor(kGray+3);line_roc->SetLineStyle(9);
  line_roc->Draw();

  TLatex latexroc;
  latexroc.SetTextSize(0.025);
  latexroc.SetTextAlign(10); 
  latexroc.DrawLatex(900,roc_sm,"SM");
  
  croc->SaveAs("plots/roc.png");

  //---------------------------------------------------------------------------------//

  TCanvas *csep = new TCanvas("csep");
  csep->SetTickx(); csep->SetTicky();
  
  TGraph *gseptrain = new TGraph((int)mass.size(),&mass[0],&v_sep_train[0]);
  TGraph *gseptest = new TGraph((int)mass.size(),&mass[0],&v_sep_test[0]);
  TGraph *gsepcv = new TGraph((int)mass.size(),&mass[0],&v_sep_cv[0]);
  TGraph *gsepsm = new TGraph((int)mass.size(),&mass[0],&v_sep_sm[0]);
  gseptrain->SetLineColor(kAzure-3);gseptrain->SetLineWidth(4);
  gseptrain->SetMarkerColor(kAzure-3);gseptrain->SetMarkerStyle(20);
  gseptest->SetLineColor(kOrange+1);gseptest->SetLineWidth(4);
  gseptest->SetMarkerColor(kOrange+1);gseptest->SetMarkerStyle(25);
  gsepcv->SetLineColor(kRed-4);gsepcv->SetLineWidth(4);gsepcv->SetLineStyle(2);
  gsepcv->SetMarkerColor(kRed-4);gsepcv->SetMarkerStyle(5);
  gsepsm->SetLineColor(kGray+3);gsepsm->SetLineWidth(3);gsepsm->SetLineStyle(9);
  gsepsm->SetMarkerColor(kGray+3);gsepsm->SetMarkerStyle(22);

  double max_sep = *max_element(v_sep_cv.begin(), v_sep_cv.end());
  double min_sep = *min_element(v_sep_cv.begin(), v_sep_cv.end());

  if (sep_sm<min_sep) min_sep=sep_sm;

  TH2F *hcan_sep = new TH2F("hcan_sep","",100,300,1100,100,0.92*min_sep,1.08*max_sep);
  hcan_sep->GetXaxis()->SetTitle("Mass");
  hcan_sep->GetYaxis()->SetTitle("SEP");
  hcan_sep->Draw();

  gsepcv->Draw("lp same");
  gseptrain->Draw("lp same");
  gseptest->Draw("lp same");
  gsepsm->Draw("lp same");
  
  TLegend *l_sep = new TLegend(0.140375,0.746269,0.457226,0.922886);
  l_sep->SetLineWidth(0);
  l_sep->SetFillStyle(0);
  l_sep->AddEntry(gseptrain,"Train","lp");
  l_sep->AddEntry(gseptest,"Test","lp");
  l_sep->AddEntry(gsepcv,"Cross-validation","lp");
  l_sep->AddEntry(gsepsm,"SM 4top","lp");
  l_sep->Draw();

  TLine *line_sep = new TLine(400,sep_sm,1000,sep_sm);
  line_sep->SetLineWidth(3);line_sep->SetLineColor(kGray+3);line_sep->SetLineStyle(9);
  line_sep->Draw();

  TLatex latexsep;
  latexsep.SetTextSize(0.025);
  latexsep.SetTextAlign(10);  //align at top
  latexsep.DrawLatex(900,sep_sm,"SM");
  
  csep->SaveAs("plots/sep.png");

  cout << "{" << flush;
  for (auto x : v_roc_cv)
    cout << x << "," << flush;
  cout << "};" << endl;

}
