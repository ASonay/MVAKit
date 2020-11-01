
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
  
  double sfac = bin/(xmax-xmin);
  hist->Scale(sfac/hist->Integral());
  
  return hist;
}

void draw_bdt_test_train(string name, TH1F* S_t, TH1F* B_t, TH1F* S_tr, TH1F* B_tr, int bin=80, double xmin=-0.93, double xmax=0.92)
{

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

  hcan->Draw();
  S_t->Draw("hist same");
  B_t->Draw("hist same");
  S_tr->Draw("p same");
  B_tr->Draw("p same");
  
  TLegend *l_1 = new TLegend(0.0740375,0.823383,0.537019,0.997512);
  l_1->SetLineWidth(3);
  l_1->SetFillStyle(0);
  l_1->SetHeader("      Training Samples");
  l_1->AddEntry(S_t,"Signal","f");
  l_1->AddEntry(B_t,"Background","f");
  l_1->Draw();
  TLegend *l_2 = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_2->SetLineWidth(3);
  l_2->SetFillStyle(0);
  l_2->SetHeader("      Test Samples");//l_2->SetHeader("      Without weighting");
  l_2->AddEntry(S_tr,"Signal","p");
  l_2->AddEntry(B_tr,"Background","p");
  l_2->Draw();
  c->SaveAs(("plots/bdt0test_train_"+name+".png").c_str());

}

void ana_single(string filename,string path="loader_0/")
{

  string class_name = "Score";
  string cut = "1";

  TFile *f = new TFile(filename.c_str());

  string testTree_name = path+"TestTree";
  string trainTree_name = path+"TrainTree";
  
  TTree *t_test = (TTree*)f->Get(testTree_name.c_str());
  TTree *t_train = (TTree*)f->Get(trainTree_name.c_str());

  string cut_exp_bkg = "(classID&&"+cut+")*weight";
  TH1F *hBkg_test = getHist(t_test,"htestBkg",class_name.c_str(),cut_exp_bkg);
  TH1F *hBkg_train = getHist(t_train,"htrainBkg",class_name.c_str(),cut_exp_bkg);

  string cut_exp_sig = "(!classID&&"+cut+")*weight";
  TH1F *hSig_test = getHist(t_test,"htestSig",class_name.c_str(),cut_exp_sig);
  TH1F *hSig_train = getHist(t_train,"htrainSig",class_name.c_str(),cut_exp_sig);
  draw_bdt_test_train("single",hSig_train,hBkg_train,hSig_test,hBkg_test);

  TMVA::ROCCalc calc_train(hSig_train,hBkg_train);
  TMVA::ROCCalc calc_test(hSig_test,hBkg_test);

  double roc_train = calc_train.GetROCIntegral();
  double roc_test = calc_test.GetROCIntegral();
  cout << " ROC Train: " << roc_train << " ROC Test: " << roc_test << endl;

  double sep_train = GetSeparation(hSig_train,hBkg_train);
  double sep_test = GetSeparation(hSig_test,hBkg_test);
  cout << " SEP Train: " << sep_train << " SEP Test: " << sep_test << endl;

  TH1 * hroc_train = calc_train.GetROC();
  TH1 * hroc_test = calc_test.GetROC();

  TCanvas *croc = new TCanvas("croc");

  hroc_train->SetLineColor(kAzure+3);
  hroc_train->Draw("hist L");
  hroc_test->SetLineColor(kRed-4);
  hroc_test->Draw("hist L same");

  croc->SaveAs("plots/croc_single.png");
  
}
