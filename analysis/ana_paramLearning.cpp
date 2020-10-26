
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
  
  return hist;
}

void draw_bdt_test_train(string name, TH1F* S_t, TH1F* B_t, TH1F* S_tr, TH1F* B_tr, int bin=80, double xmin=-0.93, double xmax=0.92)
{
  double sfac = bin/(xmax-xmin);
  S_t->Scale(sfac/S_t->Integral());
  B_t->Scale(sfac/B_t->Integral());
  S_tr->Scale(sfac/S_tr->Integral());
  B_tr->Scale(sfac/B_tr->Integral());
  
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

void ana_paramLearning(string filename,string path="loader_0/")
{

  string class_name = "Score";
  
  TFile *f = new TFile(filename.c_str());

  string testTree_name = path+"TestTree";
  string trainTree_name = path+"TrainTree";
  
  TTree *t_test = (TTree*)f->Get(testTree_name.c_str());
  TTree *t_train = (TTree*)f->Get(trainTree_name.c_str());


  vector<string> MP = {"400","500","600","700","800","900","1000"};

  vector<double> mass = {400,500,600,700,800,900,1000};
  vector<double> v_roc_train;
  vector<double> v_roc_test;
  vector<double> v_sep_train;
  vector<double> v_sep_test;
  
  for (auto mp : MP){
    string cut_exp_bkg = "(classID&&ttH_tttt_m=="+mp+")*weight";
    TH1F *hBkg_test = getHist(t_test,"htestBkg",class_name.c_str(),cut_exp_bkg);
    TH1F *hBkg_train = getHist(t_train,"htrainBkg",class_name.c_str(),cut_exp_bkg);
    string cut_exp_sig = "(!classID&&ttH_tttt_m=="+mp+")*weight";
    TH1F *hSig_test = getHist(t_test,"htestSig"+mp,class_name.c_str(),cut_exp_sig);
    TH1F *hSig_train = getHist(t_train,"htrainSig"+mp,class_name.c_str(),cut_exp_sig);
    draw_bdt_test_train(mp,hSig_train,hBkg_train,hSig_test,hBkg_test);

    TMVA::ROCCalc calc_train(hSig_train,hBkg_train);
    double roc_train = calc_train.GetROCIntegral();
    calc_train.ResetStatus();
    TMVA::ROCCalc calc_test(hSig_test,hBkg_test);
    double roc_test = calc_test.GetROCIntegral();
    calc_test.ResetStatus();

    cout << "Param: " << mp << " ROC Train: " << roc_train << " ROC Test: " << roc_test << endl;

    double sep_train = GetSeparation(hSig_train,hBkg_train);
    double sep_test = GetSeparation(hSig_test,hBkg_test);
    cout << " SEP Train: " << sep_train << " SEP Test: " << sep_test << endl;

    v_roc_train.push_back(roc_train);v_roc_test.push_back(roc_test);
    v_sep_train.push_back(sep_train);v_sep_test.push_back(sep_test);
    
  }

  TCanvas *croc = new TCanvas("croc");
  croc->SetTickx(); croc->SetTicky();
  
  TGraph *groctrain = new TGraph((int)mass.size(),&mass[0],&v_roc_train[0]);
  TGraph *groctest = new TGraph((int)mass.size(),&mass[0],&v_roc_test[0]);
  groctrain->SetLineColor(kAzure-3);groctrain->SetLineWidth(4);
  groctrain->SetMarkerColor(kAzure-3);groctrain->SetMarkerStyle(20);
  groctest->SetLineColor(kOrange+1);groctest->SetLineWidth(4);
  groctest->SetMarkerColor(kOrange+1);groctest->SetMarkerStyle(5);

  groctrain->GetXaxis()->SetTitle("Mass");
  groctrain->GetYaxis()->SetTitle("AUC");
  groctrain->SetTitle("");

  groctrain->Draw("alp");
  groctest->Draw("lp same");
  
  TLegend *l_roc = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_roc->SetLineWidth(3);
  l_roc->SetFillStyle(0);
  l_roc->AddEntry(groctrain,"Train","lp");
  l_roc->AddEntry(groctest,"Test","lp");
  l_roc->Draw();

  croc->SaveAs("plots/roc.png");

  //---------------------------------------------------------------------------------//
  
  TCanvas *csep = new TCanvas("csep");
  csep->SetTickx(); csep->SetTicky();
  
  TGraph *gseptrain = new TGraph((int)mass.size(),&mass[0],&v_sep_train[0]);
  TGraph *gseptest = new TGraph((int)mass.size(),&mass[0],&v_sep_test[0]);
  gseptrain->SetLineColor(kAzure-3);gseptrain->SetLineWidth(4);
  gseptrain->SetMarkerColor(kAzure-3);gseptrain->SetMarkerStyle(20);
  gseptest->SetLineColor(kOrange+1);gseptest->SetLineWidth(4);
  gseptest->SetMarkerColor(kOrange+1);gseptest->SetMarkerStyle(5);

  gseptrain->GetXaxis()->SetTitle("Mass");
  gseptrain->GetYaxis()->SetTitle("AUC");
  gseptrain->SetTitle("");

  gseptrain->Draw("alp");
  gseptest->Draw("lp same");
  
  TLegend *l_sep = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_sep->SetLineWidth(3);
  l_sep->SetFillStyle(0);
  l_sep->AddEntry(gseptrain,"Train","lp");
  l_sep->AddEntry(gseptest,"Test","lp");
  l_sep->Draw();

  csep->SaveAs("plots/sep.png");
  
}
