int binx = 18;

unique_ptr<TH1F> getHist(TTree *tree,string hname, string var, string select, int bin=binx, double xmin=100, double xmax=1000);


void fit()
{
  TFile *f = new TFile("pyMVAKit.root");
  TTree *tr_train = (TTree*)f->Get("TrainTree0");
  TTree *tr_test = (TTree*)f->Get("TestTree0");

  unique_ptr<TH1F> h_data = getHist(tr_train,"hdata","x","(classID)*weight");
  h_data->Add(getHist(tr_test,"hdata2","x","(classID)*weight").get());
  unique_ptr<TH1F> h_mc = getHist(tr_train,"hmc","x","(!classID)*weight");
  h_mc->Add(getHist(tr_test,"hmc2","x","(!classID)*weight").get());
  unique_ptr<TH1F> h_mcx = getHist(tr_train,"hmcx","x","(!classID)*weight*((score[0])/(1.-score[0]))");
  h_mcx->Add(getHist(tr_test,"hmcx2","x","(!classID)*weight*((score[0])/(1.-score[0]))").get());
  unique_ptr<TH1F> h_mcx_up = getHist(tr_train,"hmcx_up","x","(!classID)*weight*((score[0]+score[2])/(1.-score[0]-score[2]))");
  h_mcx_up->Add(getHist(tr_test,"hmcx_up2","x","(!classID)*weight*((score[0]+score[2])/(1.-score[0]-score[2]))").get());

  TGraph *gr_nn = (TGraph*)f->Get("g");
  TGraph *gr_nn_up = (TGraph*)f->Get("gup");
  

  h_data->SetMarkerStyle(20); h_data->SetMarkerColor(kGray+3); h_data->SetLineColor(kGray+3); h_data->SetLineWidth(4); h_data->SetLineStyle(1);
  h_mc->SetMarkerStyle(23); h_mc->SetMarkerColor(kRed-4); h_mc->SetLineColor(kRed-4); h_mc->SetLineWidth(4); h_mc->SetLineStyle(2);
  h_mcx->SetMarkerStyle(23); h_mcx->SetMarkerColor(kAzure-3); h_mcx->SetLineColor(kAzure-3); h_mcx->SetLineWidth(4); h_mcx->SetLineStyle(1);
  h_mcx_up->SetMarkerStyle(23); h_mcx_up->SetMarkerColor(kRed-4); h_mcx_up->SetLineColor(kRed-4); h_mcx_up->SetLineWidth(4); h_mcx_up->SetLineStyle(1);


  auto hdiff = unique_ptr<TH1F>((TH1F*)h_data->Clone("hdiff")); hdiff->Divide(h_mc.get());
  hdiff->SetMarkerStyle(20); hdiff->SetMarkerColor(kGray+3); hdiff->SetLineColor(kGray+3); hdiff->SetLineWidth(3); hdiff->SetMarkerSize(1.5);
  
  TF1 *fun = new TF1("fun","[0]*exp([1]*x)",100,1000);
  TF1 *fun_err = new TF1("fun_err","[0]*exp([1]*x)",100,1000);
  fun->SetParameters(1.0,0.001);
  hdiff->Fit(fun,"0");
  fun_err->SetParameters(fun->GetParameter(0)+fun->GetParError(0),
			 fun->GetParameter(1)+fun->GetParError(1));
  
  auto h_mcxx = make_unique<TH1F>("h_mcxx","",binx,100,1000);
  for (int i=0;i<binx;i++){
    float x = fun->Eval(h_mc->GetBinCenter(i+1));
    float x_up = fun_err->Eval(h_mc->GetBinCenter(i+1));
    float con = h_mc->GetBinContent(i+1);
    h_mcxx->SetBinContent(i+1,con*x);
    h_mcxx->SetBinError(i+1,con*abs(x_up-x));

    float xnn = h_mcx->GetBinContent(i+1);
    float xnn_up = h_mcx_up->GetBinContent(i+1);
    h_mcx->SetBinError(i+1,abs(xnn_up-xnn));
  }
  h_mcxx->SetMarkerStyle(23); h_mcxx->SetMarkerColor(kOrange+1); h_mcxx->SetLineColor(kOrange+1); h_mcxx->SetLineWidth(4); h_mcxx->SetLineStyle(9);

  auto hdiff2 = unique_ptr<TH1F>((TH1F*)h_data->Clone("hdiff2")); hdiff2->Divide(h_mcx.get());
  hdiff2->SetMarkerStyle(20); hdiff2->SetMarkerColor(kAzure-3); hdiff2->SetLineColor(kAzure-3); hdiff2->SetLineWidth(3); hdiff2->SetMarkerSize(1.5);
  auto hdiff2_unc = unique_ptr<TH1F>((TH1F*)hdiff2->Clone("hdiff2_unc"));
  hdiff2_unc->SetFillStyle(3001); hdiff2_unc->SetFillColor(kAzure-3);

  auto hdiff3 = unique_ptr<TH1F>((TH1F*)h_data->Clone("hdiff3")); hdiff3->Divide(h_mcxx.get());
  hdiff3->SetMarkerStyle(24); hdiff3->SetMarkerColor(kOrange+1); hdiff3->SetLineColor(kOrange+1); hdiff3->SetLineWidth(3); hdiff3->SetMarkerSize(1.5);
  auto hdiff3_unc = unique_ptr<TH1F>((TH1F*)hdiff3->Clone("hdiff3_unc"));
  hdiff3_unc->SetFillStyle(3001); hdiff3_unc->SetFillColor(kOrange+1);
  for (int i=0;i<binx;i++){
    hdiff2->SetBinError(i+1,0.0001);
    hdiff3->SetBinError(i+1,0.0001);
  }
  
  
  
  TCanvas *c = new TCanvas("c");
  TPad *pad = new TPad("pad","",0,0.3,1,1.0);
  pad->SetTickx(); pad->SetTicky(); //pad->SetLogy();
  pad->SetBottomMargin(0);
  pad->Draw();
  pad->cd();

  double ymin = 1e32;
  for (int i=0;i<binx;i++)
    if (ymin>h_data->GetBinContent(i+1)&&h_data->GetBinContent(i+1)>0.0)
      ymin = h_data->GetBinContent(i+1);
  for (int i=0;i<binx;i++)
    if (ymin>h_mc->GetBinContent(i+1)&&h_mc->GetBinContent(i+1)>0.0)
      ymin = h_mc->GetBinContent(i+1);
  double ymax = -1e32;
  for (int i=0;i<binx;i++)
    if (ymax<h_data->GetBinContent(i+1))
      ymax = h_data->GetBinContent(i+1);
  for (int i=0;i<binx;i++)
    if (ymax<h_mc->GetBinContent(i+1))
      ymax = h_mc->GetBinContent(i+1);

  cout << "YMIN: " << ymin << " YMAX: " << ymax << endl; 
  
  TH2F *hcan_in = new TH2F("hcan_in","",100,h_data->GetXaxis()->GetXmin(),h_data->GetXaxis()->GetXmax(),100,0.75*ymin,1.05*ymax);
  hcan_in->GetYaxis()->SetTitle("Normalized Count ");

  hcan_in->Draw();
  h_mc->Draw("hist same");
  h_mcx->Draw("hist same");
  h_mcxx->Draw("hist same");
  h_data->Draw("e1 same");

  TLegend *l_1 = new TLegend(0.513919,0.7086,0.926772,0.92467);
  l_1->SetLineWidth(0);
  l_1->SetFillStyle(0);
  l_1->AddEntry(h_data.get(),"Data","l");
  l_1->AddEntry(h_mc.get(),"MC","l");
  l_1->AddEntry(h_mcx.get(),"MC Corr. NN","l");
  l_1->AddEntry(h_mcxx.get(),"MC Corr. Fit","l");
  l_1->Draw();


  c->Draw();
  c->cd();
  TPad *overlay = new TPad("overlay","",0.,0.05,1,0.3);
  overlay->SetTopMargin(0);
  overlay->SetBottomMargin(0.2); overlay->Draw();
  overlay->cd();
  overlay->SetTickx(); overlay->SetTicky(); 

  TH2F *hcan = new TH2F("hcan","",100,hdiff->GetXaxis()->GetXmin(),hdiff->GetXaxis()->GetXmax(),100,0.4,1.6);

  hcan->Draw();
  
  //hdiff->Draw("e1 same");
  hdiff3_unc->Draw("e3 same");
  hdiff2_unc->Draw("e3 same");
  fun->SetLineColor(kRed-4); fun->SetLineStyle(2);
  //fun->Draw("same");
  hdiff2->Draw("e1 same");
  hdiff3->Draw("e1 same");

  TLine *lin1 = new TLine(hdiff->GetXaxis()->GetXmin(),1.0,hdiff->GetXaxis()->GetXmax(),1.0);
  lin1->SetLineColor(kGray+3); lin1->SetLineStyle(9); lin1->SetLineWidth(2);
  lin1->Draw();

  // Y axis h_data plot settings
  h_data->GetYaxis()->SetTitleSize(20);
  h_data->GetYaxis()->SetTitleFont(43);
  h_data->GetYaxis()->SetTitleOffset(1.55);

  // Ratio plot (hdiff) settings
  hdiff->SetTitle(""); // Remove the ratio title

  // Y axis ratio plot settings
  hcan->GetYaxis()->SetTitle("Data/Pred");
  hcan->GetYaxis()->SetNdivisions(505);
  hcan->GetYaxis()->SetTitleSize(30);
  hcan->GetYaxis()->SetTitleFont(63);
  hcan->GetYaxis()->SetTitleOffset(0.8);
  hcan->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hcan->GetYaxis()->SetLabelSize(15);

  // X axis ratio plot settings
  hcan->GetXaxis()->SetTitle("X");
  hcan->GetXaxis()->SetTitleSize(30);
  hcan->GetXaxis()->SetTitleFont(63);
  hcan->GetXaxis()->SetTitleOffset(2.4);
  hcan->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
  hcan->GetXaxis()->SetLabelSize(15);

  c->SaveAs("plots/toy.png");

  //fit----------------------
  TCanvas *c_fit = new TCanvas("c_fit");
  c_fit->SetTickx(); c_fit->SetTicky();
  
  TH2F *hcan_fit = new TH2F("hcan_fit","",100,hdiff->GetXaxis()->GetXmin(),hdiff->GetXaxis()->GetXmax(),100,0.001,2.1);
  hcan_fit->GetXaxis()->SetTitle("X");
  hcan_fit->GetYaxis()->SetTitle("Data/Pred.");
  hcan_fit->Draw();

  int n = 100;
  vector<float> x_fit,y_fit,y_fit_err;

  for (int i=0;i<n;i++){
    x_fit.push_back(100.+i*(1000.-100.)/n);
    y_fit.push_back(fun->Eval(x_fit.back()));
    y_fit_err.push_back(fun_err->Eval(x_fit.back())-y_fit.back());
  }

  auto gr_fit_unc = new TGraphErrors(n,&x_fit[0],&y_fit[0],0,&y_fit_err[0]);
  gr_fit_unc->SetFillStyle(3001); gr_fit_unc->SetFillColor(kAzure-3); gr_fit_unc->SetLineColor(kAzure-3);
  gr_fit_unc->Draw("e3 l same");
  hdiff->Draw("e1 same");

  TLatex latex_fit;
  latex_fit.SetTextSize(0.04);
  latex_fit.SetTextAlign(13);  //align at top
  latex_fit.SetTextColor(kAzure-3);
  latex_fit.DrawLatex(700,1.5,Form("%1.1f e^{%1.1e x}",fun->GetParameter(0),fun->GetParameter(1)));
  
  c_fit->SaveAs("plots/toy_fit.png");

  //nn----------------------
  TCanvas *c_nn = new TCanvas("c_nn");
  c_nn->SetTickx(); c_nn->SetTicky();

  TH2F *hcan_nn = new TH2F("hcan_nn","",100,hdiff->GetXaxis()->GetXmin(),hdiff->GetXaxis()->GetXmax(),100,0.001,2.1);
  hcan_nn->GetXaxis()->SetTitle("X");
  hcan_nn->GetYaxis()->SetTitle("Data/Pred.");
  hcan_nn->Draw();

  vector<float> x_nn,y_nn,y_nn_err;
  
  for (int i=0;i<gr_nn->GetN();i++){
    Double_t x, y;
    gr_nn->GetPoint(i,x,y);
    x_nn.push_back(x);
    y_nn.push_back(y);
    gr_nn_up->GetPoint(i,x,y);
    y_nn_err.push_back(y-y_nn.back());
  }

  auto gr_nn_unc = new TGraphErrors(gr_nn->GetN(),&x_nn[0],&y_nn[0],0,&y_nn_err[0]);
  gr_nn_unc->SetFillStyle(3001); gr_nn_unc->SetFillColor(kOrange+1); gr_nn_unc->SetLineColor(kOrange+1);
  gr_nn_unc->Draw("e3 l same");
  hdiff->Draw("e1 same");
  
  TLatex latex_nn;
  latex_nn.SetTextSize(0.04);
  latex_nn.SetTextAlign(13);  //align at top
  latex_nn.SetTextColor(kOrange+1);
  latex_nn.DrawLatex(700,1.5,"NN(x)");
  
  c_nn->SaveAs("plots/toy_nn.png");

}


unique_ptr<TH1F> getHist(TTree *tree,string hname, string var, string select, int bin=100, double xmin=-0.1, double xmax=1.1)
{
  
  string name = hname+var+select;
  vector<pair<char,char>> rc = {{'/','_'},{'+', '_'},{'-', '_'},{'*', '_'},{'(', '_'},{'$', '_'},{')', '_'},{'>', '_'},{'<', '_'},{'=', '_'},{'[', '_'},{']', '_'},{'<', 's'},{'>', 'g'},{'+', '_'},{'-', '_'},{'.', '_'},{'&', '_'}};
  for (auto x : rc) {replace( name.begin(), name.end(), x.first, x.second);}

  auto hist = make_unique<TH1F>(name.c_str(),"",bin,xmin,xmax);
  tree->Project(name.c_str(),var.c_str(),select.c_str());

  return hist;
}
