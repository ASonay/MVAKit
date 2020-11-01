
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

TH1F* getHist(TTree *tree,string hname, string var, string select, int bin=80, double xmin=-1.2, double xmax=1.2)
{
  cout << "Variable: " << var << endl;
  cout << "Selection: " << select << endl;

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

  cout << "Entries : " << hist->GetEntries() << endl;
  
  return hist;
}


void draw_test_train_val(string name, string cut, TH1F* S_t, TH1F* B_t, TH1F* S_tr, TH1F* B_tr, TH1F* S_cv, TH1F* B_cv, int bin=80, double xmin=-0.93, double xmax=0.92)
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
  hcan->GetXaxis()->SetTitle(name.c_str());
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

void draw_train_test(TH1F* S_t, TH1F* B_t, TH1F* S_tr, TH1F* B_tr,string mp,string var,string name)
{
  double bin = S_t->GetNbinsX();
  double xmin = S_t->GetXaxis()->GetXmin();
  double xmax = S_t->GetXaxis()->GetXmax();
  double sfac = bin/(xmax-xmin);;
  S_t->Scale(sfac/S_t->Integral());
  B_t->Scale(sfac/B_t->Integral());
  S_tr->Scale(sfac/S_tr->Integral());
  B_tr->Scale(sfac/B_tr->Integral());
  
  double ymin = 1e32;
  for (int i=0;i<S_t->GetNbinsX();i++)
    if (ymin>S_t->GetBinContent(i+1)&&S_t->GetBinContent(i+1)>0.0)
      ymin = S_t->GetBinContent(i+1);
  for (int i=0;i<S_t->GetNbinsX();i++)
    if (ymin>B_t->GetBinContent(i+1)&&B_t->GetBinContent(i+1)>0.0)
      ymin = B_t->GetBinContent(i+1);
  double ymax = -1e32;
  for (int i=0;i<S_t->GetNbinsX();i++)
    if (ymax<S_t->GetBinContent(i+1))
      ymax = S_t->GetBinContent(i+1);
  for (int i=0;i<S_t->GetNbinsX();i++)
    if (ymax<B_t->GetBinContent(i+1))
      ymax = B_t->GetBinContent(i+1);

  TH2F *hcan = new TH2F(("hcan"+var+mp+name).c_str(),"",100,S_t->GetXaxis()->GetXmin(),S_t->GetXaxis()->GetXmax(),100,ymin,1.5*ymax);
  hcan->GetXaxis()->SetTitle(var.c_str());
  hcan->GetYaxis()->SetTitle("1/N (dN/dx)");

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
  l_1->SetHeader(("Training Samples for: M"+mp).c_str());
  l_1->AddEntry(S_t,"Signal","f");
  l_1->AddEntry(B_t,"Background","f");
  l_1->Draw();
  TLegend *l_2 = new TLegend(0.538993,0.823383,0.994077,0.997512);
  l_2->SetLineWidth(3);
  l_2->SetFillStyle(0);
  l_2->SetHeader(("Testing Samples for: M"+mp).c_str());
  l_2->AddEntry(S_tr,"Signal","p");
  l_2->AddEntry(B_tr,"Background","p");
  l_2->Draw();


}
