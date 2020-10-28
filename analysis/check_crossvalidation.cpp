
void draw_bdt_comp(string name, TH1F* S_t, TH1F* S_tr, int bin=80, double xmin=-0.93, double xmax=0.92)
{
  double sfac = bin/(xmax-xmin);
  S_t->Scale(sfac/S_t->Integral());
  S_tr->Scale(sfac/S_tr->Integral());
  
  double ymin = 1e32;
  for (int i=0;i<bin;i++)
    if (ymin>S_t->GetBinContent(i+1)&&S_t->GetBinContent(i+1)>0.0)
      ymin = S_t->GetBinContent(i+1);
  for (int i=0;i<bin;i++)
    if (ymin>S_tr->GetBinContent(i+1)&&S_tr->GetBinContent(i+1)>0.0)
      ymin = S_tr->GetBinContent(i+1);
  double ymax = -1e32;
  for (int i=0;i<bin;i++)
    if (ymax<S_t->GetBinContent(i+1))
      ymax = S_t->GetBinContent(i+1);
  for (int i=0;i<bin;i++)
    if (ymax<S_tr->GetBinContent(i+1))
      ymax = S_tr->GetBinContent(i+1);

  TH2F *hcan = new TH2F("hcan","",100,S_t->GetXaxis()->GetXmin(),S_t->GetXaxis()->GetXmax(),100,ymin,1.5*ymax);
  hcan->GetXaxis()->SetTitle("BDT Output");
  hcan->GetYaxis()->SetTitle("1/N (dN/dx)");


  TCanvas *c = new TCanvas(S_t->GetName());
  c->SetTicky(); c->SetTickx();

  S_t->SetFillStyle(1001); S_t->SetFillColor(kAzure+7); S_t->SetLineColor(kAzure+7); S_t->SetLineWidth(3);

  S_tr->SetMarkerStyle(20); S_tr->SetMarkerSize(1.7); S_tr->SetMarkerColor(4); S_tr->SetLineColor(4); S_tr->SetLineWidth(3);

  hcan->Draw();
  S_t->Draw("hist same");
  S_tr->Draw("p same");
  
  TLegend *l_1 = new TLegend(0.0740375,0.823383,0.537019,0.997512);
  l_1->SetLineWidth(3);
  l_1->SetFillStyle(0);
  l_1->SetHeader("      Eval Samples");
  l_1->AddEntry(S_t,"Signal","f");
  l_1->Draw();
  TLegend *l_2 = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_2->SetLineWidth(3);
  l_2->SetFillStyle(0);
  l_2->SetHeader("      Test Samples");//l_2->SetHeader("      Without weighting");
  l_2->AddEntry(S_tr,"Signal","p");
  l_2->Draw();
  c->SaveAs(("plots/comp_test_eval_"+name+".png").c_str());

}

TH1F* getHist(TTree *tree,string hname, string var, string select, int bin=80, double xmin=-0.93, double xmax=0.92)
{

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

void check_crossvalidation()
{

  string file_loc = "/eos/user/a/asonay/HBSM4top_ntuple/";
  string file_name = "ttH_tttt_m";
  vector<string> suffix = {"ljetsge10j"};
  vector<string> mc_samp = {"mc16a","mc16d","mc16e"};
  vector<string> masses = {"400"};

  TChain *chain = new TChain("nominal_Loose");

  for (auto suf : suffix){
    for (auto mass : masses){
      for (auto mc : mc_samp){
	string fullname = file_loc+suf+"/"+file_name+mass+"_"+mc+".root";
	cout << fullname << endl;
	chain->Add(fullname.c_str());
      }
    }
  }
  TTree *tr_merged = chain;

  TFile *f1 = new TFile("/afs/cern.ch/work/a/asonay/bsm4top_bdt_weight_cv/ljets/factory_BDT_0.root");
  TFile *f2 = new TFile("/afs/cern.ch/work/a/asonay/bsm4top_bdt_weight_cv/ljets/factory_BDT_1.root");
  TTree *tree_train_1 = (TTree*)f1->Get("loader_0/TrainTree");
  TTree *tree_test_1 = (TTree*)f1->Get("loader_0/TestTree");
  TTree *tree_train_2 = (TTree*)f2->Get("loader_1/TrainTree");
  TTree *tree_test_2 = (TTree*)f2->Get("loader_1/TestTree");

  TH1F *hSig_1 = getHist(tr_merged,"hSig_1","BDT_score_ttH_tttt_m500","(nBTags_MV2c10_70>=3)*((3219.56+32988.1)*(runNumber==284500)+44307.4*(runNumber==300000)+58450.1*(runNumber==310000))*(weight_normalise_merged*weight_mc*weight_pileup*weight_leptonSF*weight_jvt*weight_bTagSF_MV2c10_Continuous)");

  TH1F *hSig_2 = getHist(tree_test_1,"hSig_2","Score","(!classID&&ttH_tttt_m==400&&nJets>=10)*weight");
  hSig_2->Add(getHist(tree_test_2,"hSig_2_0","Score","(!classID&&ttH_tttt_m==400&&nJets>=10)*weight"));

  draw_bdt_comp("comp",hSig_1,hSig_2);

  
  
}
