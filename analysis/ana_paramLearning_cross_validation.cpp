
double GetSeparation(TH1* fmvaS, TH1* fmvaB){
  
  int fNbins = fmvaS->GetNbinsX();
  double fXmax = fmvaS->GetXaxis()->GetXmax();
  double fXmin = fmvaS->GetXaxis()->GetXmin(); 

  double s,b,sum=0;
  double dx = (fXmax-fXmin)/(double)fNbins;
  for (int i=0;i<fNbins;i++){
    s=fmvaS->GetBinContent(i+1)/(fmvaS->GetSumOfWeights());
    b=fmvaB->GetBinContent(i+1)/(fmvaB->GetSumOfWeights());
    if (s+b>0)
      sum+= (0.5*(s-b)*(s-b)/(s+b));
  }

  return sum;

}
TH1F* getHist(TTree *tree,string hname, string var, string select, int bin=80, double xmin=-0.93, double xmax=0.92)
{
  cout << var << endl;
  cout << select << endl;

  cout << "Selection Entries : " << tree->GetEntries(select.c_str()) << endl;
  
  string name = hname+var+select;
  replace( name.begin(), name.end(), '/', '_');
  replace( name.begin(), name.end(), '+', '_');
  replace( name.begin(), name.end(), '-', '_');
  replace( name.begin(), name.end(), '*', '_');
  replace( name.begin(), name.end(), '(', '_');
  replace( name.begin(), name.end(), '$', '_');
  replace( name.begin(), name.end(), ')', '_');
  replace( name.begin(), name.end(), '>', '_');
  replace( name.begin(), name.end(), '<', '_');
  replace( name.begin(), name.end(), '=', '_');
  replace( name.begin(), name.end(), '[', '_');
  replace( name.begin(), name.end(), ']', '_');
  replace( name.begin(), name.end(), '<', 's');
  replace( name.begin(), name.end(), '>', 'g');
  replace( name.begin(), name.end(), '+', '_');
  replace( name.begin(), name.end(), '-', '_');
  replace( name.begin(), name.end(), '.', '_');
  replace( name.begin(), name.end(), '&', '_');

  TH1F *hist = new TH1F(name.c_str(),"",bin,xmin,xmax);
  tree->Project(name.c_str(),var.c_str(),select.c_str());

  cout << "Histogram entries : " << hist->GetEntries() << endl;
  
  return hist;
}

void draw_bdt_test_train(string name, string cut, TH1F* S_t, TH1F* B_t, TH1F* S_tr, TH1F* B_tr, TH1F* S_cv, TH1F* B_cv, int bin=80, double xmin=-0.93, double xmax=0.92)
{
  double sfac = bin/(xmax-xmin);
  S_t->Scale(sfac/S_t->Integral());
  B_t->Scale(sfac/B_t->Integral());
  S_tr->Scale(sfac/S_tr->Integral());
  B_tr->Scale(sfac/B_tr->Integral());
  S_cv->Scale(sfac/S_cv->Integral());
  B_cv->Scale(sfac/B_cv->Integral());
  
  double ymin = 1e32;
  for (int i=0;i<bin;i++)
    if (ymin>S_t->GetBinContent(i+1)&&S_t->GetBinContent(i+1)>0.0)
      ymin = S_t->GetBinContent(i+1);
  for (int i=0;i<bin;i++)
    if (ymin>B_t->GetBinContent(i+1)&&B_t->GetBinContent(i+1)>0.0)
      ymin = B_t->GetBinContent(i+1);
  double ymax = -1e32;
  for (int i=0;i<bin;i++)
    if (ymax<S_t->GetBinContent(i+1))
      ymax = S_t->GetBinContent(i+1);
  for (int i=0;i<bin;i++)
    if (ymax<B_t->GetBinContent(i+1))
      ymax = B_t->GetBinContent(i+1);

  TH2F *hcan = new TH2F("hcan","",100,S_t->GetXaxis()->GetXmin(),S_t->GetXaxis()->GetXmax(),100,ymin,1.5*ymax);
  hcan->GetXaxis()->SetTitle("BDT Output");
  hcan->GetYaxis()->SetTitle("1/N (dN/dx)");


  TCanvas *c = new TCanvas(S_t->GetName());
  c->SetTicky(); c->SetTickx();

  S_t->SetFillStyle(1001); S_t->SetFillColor(kAzure+7); S_t->SetLineColor(kAzure+7); S_t->SetLineWidth(3);
  B_t->SetFillStyle(3445); B_t->SetFillColor(kRed-7); B_t->SetLineColor(kRed-7); B_t->SetLineWidth(3);

  S_tr->SetMarkerStyle(20); S_tr->SetMarkerSize(1.7); S_tr->SetMarkerColor(4); S_tr->SetLineColor(4); S_tr->SetLineWidth(3);
  B_tr->SetMarkerStyle(20); B_tr->SetMarkerSize(1.7); B_tr->SetMarkerColor(2); B_tr->SetLineColor(2); B_tr->SetLineWidth(3);
  
  S_cv->SetFillStyle(3003); S_cv->SetFillColor(kSpring-4); S_cv->SetLineColor(kSpring-4); S_cv->SetLineWidth(3);
  B_cv->SetFillStyle(3475); B_cv->SetFillColor(kOrange-4); B_cv->SetLineColor(kOrange-4); B_cv->SetLineWidth(3);

  hcan->Draw();
  S_t->Draw("hist same");
  B_t->Draw("hist same");
  S_tr->Draw("p same");
  B_tr->Draw("p same");
  S_cv->Draw("hist same");
  B_cv->Draw("hist same");
  
 
  TLegend *l_1 = new TLegend(0.0740375,0.746269,0.367226,0.922886);
  l_1->SetLineWidth(3);
  l_1->SetFillStyle(0);
  l_1->SetHeader("      Train");//l_1->SetHeader("      With weighting");
  l_1->AddEntry(S_t,("Signal @"+cut).c_str(),"f");
  l_1->AddEntry(B_t,("Background @"+cut).c_str(),"f");
  l_1->Draw();
  TLegend *l_2 = new TLegend(0.367226,0.746269,0.681145,0.922886);
  l_2->SetLineWidth(3);
  l_2->SetFillStyle(0);
  l_2->SetHeader("      Test");//l_2->SetHeader("      Without weighting");
  l_2->AddEntry(S_tr,("Signal @"+cut).c_str(),"p");
  l_2->AddEntry(B_tr,("Background @"+cut).c_str(),"p");
  l_2->Draw();
  TLegend *l_3 = new TLegend(0.681145,0.746269,0.994077,0.922886);
  l_3->SetLineWidth(3);
  l_3->SetFillStyle(0);
  l_3->SetHeader("      Cross-validation");//l_3->SetHeader("      Without weighting");
  l_3->AddEntry(S_cv,("Signal @"+cut).c_str(),"f");
  l_3->AddEntry(B_cv,("Background @"+cut).c_str(),"f");
  l_3->Draw();

  c->SaveAs(("plots/bdt0test_train_"+name+".png").c_str());

}

void ana_paramLearning_cross_validation()
{

  string class_name = "Score";
  string cut = "(nJets>=10&&nBTags_MV2c10_70>=4)";
  

  TFile *f1 = new TFile("/afs/cern.ch/work/a/asonay/bsm4top_bdt_weight_cv/ljets/factory_BDT_0.root");
  TFile *f2 = new TFile("/afs/cern.ch/work/a/asonay/bsm4top_bdt_weight_cv/ljets/factory_BDT_1.root");
  TTree *t_train_1 = (TTree*)f1->Get("loader_0/TrainTree");
  TTree *t_test_1 = (TTree*)f1->Get("loader_0/TestTree");
  TTree *t_train_2 = (TTree*)f2->Get("loader_1/TrainTree");
  TTree *t_test_2 = (TTree*)f2->Get("loader_1/TestTree");

  vector<string> MP = {"900","1000"};

  vector<double> mass = {900,1000};
  vector<double> v_roc_train;
  vector<double> v_roc_test;
  vector<double> v_roc_cv;
  vector<double> v_sep_train;
  vector<double> v_sep_test;
  vector<double> v_sep_cv;
  
  string file_loc = "/eos/user/a/asonay/HBSM4top_ntuple/";
  vector<string> suffix = {"ljets9j","ljetsge10j"};
  vector<string> mc_samp = {"mc16a","mc16d","mc16e"};

  TChain *chain_bkg = new TChain("nominal_Loose");
  for (auto suf : suffix){
    for (auto mc : mc_samp){
      string fullname_bkg = file_loc+suf+"/"+"ttbar_PhPy8_AFII_AllFilt_"+mc+".root";
      cout << fullname_bkg << endl;
      chain_bkg->Add(fullname_bkg.c_str());
    }
  }
  TTree *tr_bkg = chain_bkg;

  for (auto mp : MP){

    TChain *chain_sig = new TChain("nominal_Loose");
    for (auto suf : suffix){
      for (auto mc : mc_samp){
	string fullname_sig = file_loc+suf+"/"+"ttH_tttt_m"+mp+"_"+mc+".root";
	cout << fullname_sig << endl;
	chain_sig->Add(fullname_sig.c_str());
      }
    }
    TTree * tr_sig = chain_sig;

    TH1F *hSig_cv = getHist(tr_sig,"hSig_cv","BDT_score_ttH_tttt_m"+mp,cut+"*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");

    TH1F *hBkg_cv = getHist(tr_bkg,"hBkg_cv","BDT_score_ttH_tttt_m"+mp,cut+"*((NN_score_hf/(1.-NN_score_hf))*(NN_score_kin/(1.-NN_score_kin)))*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");
    
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
    draw_bdt_test_train(mp,cut,hSig_train,hBkg_train,hSig_test,hBkg_test,hSig_cv,hBkg_cv);

    TMVA::ROCCalc calc_train(hSig_train,hBkg_train);
    double roc_train = calc_train.GetROCIntegral();
    calc_train.ResetStatus();
    TMVA::ROCCalc calc_test(hSig_test,hBkg_test);
    double roc_test = calc_test.GetROCIntegral();
    calc_test.ResetStatus();
    TMVA::ROCCalc calc_cv(hSig_cv,hBkg_cv);
    double roc_cv = calc_cv.GetROCIntegral();
    calc_cv.ResetStatus();

    cout << "Param: " << mp << " ROC Train: " << roc_train << " ROC Test: " << roc_test << " ROC CV: " << roc_cv << endl;

    double sep_train = GetSeparation(hSig_train,hBkg_train);
    double sep_test = GetSeparation(hSig_test,hBkg_test);
    double sep_cv = GetSeparation(hSig_cv,hBkg_cv);
    cout << " SEP Train: " << sep_train << " SEP Test: " << sep_test << endl << " SEP Cv: " << sep_cv << endl;

    v_roc_train.push_back(roc_train);v_roc_test.push_back(roc_test);v_roc_cv.push_back(roc_cv);
    v_sep_train.push_back(sep_train);v_sep_test.push_back(sep_test);v_sep_cv.push_back(sep_cv);

  }

  TCanvas *croc = new TCanvas("croc");
  croc->SetTickx(); croc->SetTicky();
  
  TGraph *groctrain = new TGraph((int)mass.size(),&mass[0],&v_roc_train[0]);
  TGraph *groctest = new TGraph((int)mass.size(),&mass[0],&v_roc_test[0]);
  TGraph *groccv = new TGraph((int)mass.size(),&mass[0],&v_roc_cv[0]);
  groctrain->SetLineColor(kAzure-3);groctrain->SetLineWidth(4);
  groctrain->SetMarkerColor(kAzure-3);groctrain->SetMarkerStyle(20);
  groctest->SetLineColor(kOrange+1);groctest->SetLineWidth(4);
  groctest->SetMarkerColor(kOrange+1);groctest->SetMarkerStyle(25);
  groccv->SetLineColor(kRed-4);groccv->SetLineWidth(4);groccv->SetLineStyle(2);
  groccv->SetMarkerColor(kRed-4);groccv->SetMarkerStyle(5);

  groctrain->GetXaxis()->SetTitle("Mass");
  groctrain->GetYaxis()->SetTitle("AUC");
  groctrain->SetTitle("");

  groctrain->Draw("alp");
  groctest->Draw("lp same");
  groccv->Draw("lp same");
  
  TLegend *l_roc = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_roc->SetLineWidth(3);
  l_roc->SetFillStyle(0);
  l_roc->AddEntry(groctrain,"Train","lp");
  l_roc->AddEntry(groctest,"Test","lp");
  l_roc->AddEntry(groccv,"Cross-validation","lp");
  l_roc->Draw();

  croc->SaveAs("plots/roc.png");

  //---------------------------------------------------------------------------------//

  TCanvas *csep = new TCanvas("csep");
  csep->SetTickx(); csep->SetTicky();
  
  TGraph *gseptrain = new TGraph((int)mass.size(),&mass[0],&v_sep_train[0]);
  TGraph *gseptest = new TGraph((int)mass.size(),&mass[0],&v_sep_test[0]);
  TGraph *gsepcv = new TGraph((int)mass.size(),&mass[0],&v_sep_cv[0]);
  gseptrain->SetLineColor(kAzure-3);gseptrain->SetLineWidth(4);
  gseptrain->SetMarkerColor(kAzure-3);gseptrain->SetMarkerStyle(20);
  gseptest->SetLineColor(kOrange+1);gseptest->SetLineWidth(4);
  gseptest->SetMarkerColor(kOrange+1);gseptest->SetMarkerStyle(25);
  gsepcv->SetLineColor(kRed-4);gsepcv->SetLineWidth(4);gsepcv->SetLineStyle(2);
  gsepcv->SetMarkerColor(kRed-4);gsepcv->SetMarkerStyle(5);

  gseptrain->GetXaxis()->SetTitle("Mass");
  gseptrain->GetYaxis()->SetTitle("AUC");
  gseptrain->SetTitle("");

  gseptrain->Draw("alp");
  gseptest->Draw("lp same");
  gsepcv->Draw("lp same");
  
  TLegend *l_sep = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_sep->SetLineWidth(3);
  l_sep->SetFillStyle(0);
  l_sep->AddEntry(gseptrain,"Train","lp");
  l_sep->AddEntry(gseptest,"Test","lp");
  l_sep->AddEntry(gsepcv,"Cross-validation","lp");
  l_sep->Draw();

  csep->SaveAs("plots/sep.png");

  
}
