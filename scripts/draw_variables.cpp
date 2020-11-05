#include "helper.cpp"

void draw_variables()
{

  string original_file_path = "/eos/user/a/asonay/HBSM4top_ntuple/";
  string tmva_path = "../../bsm4top_bdt_weight_cv/ljets/enum/";
  string class_name = "Score";
  string cut = "(nBTags_MV2c10_70>=3&&nJets>=9)";
  
  TFile *f1 = new TFile((tmva_path+"factory_BDT_0.root").c_str());
  TFile *f2 = new TFile((tmva_path+"factory_BDT_1.root").c_str());
  TTree *t_train_1 = (TTree*)f1->Get("loader_0/TrainTree");
  TTree *t_test_1 = (TTree*)f1->Get("loader_0/TestTree");
  TTree *t_train_2 = (TTree*)f2->Get("loader_1/TrainTree");
  TTree *t_test_2 = (TTree*)f2->Get("loader_1/TestTree");

  vector<string> MP = {"400","500","600","700","800","900","1000"};
  
  vector<pair<string,string>> variables = {
					   {"HT_all","HT_all"},
					   {"jet_pt[0]","jet_pt_0_"},
					   {"nJets","nJets"},
					   {"Centrality_all","Centrality_all"},
					   {"dRjj_Avg","dRjj_Avg"},
					   {"dRbb_MindR_MV2c10_70","dRbb_MindR_MV2c10_70"},
					   {"dRbl_MindR_MV2c10_70","dRbl_MindR_MV2c10_70"},
					   {"Mjjj_MindR","Mjjj_MindR"},
					   {"Mbbb_Avg_MV2c10_70","Mbbb_Avg_MV2c10_70"},
					   {"Mbb_MinM_MV2c10_70","Mbb_MinM_MV2c10_70"},
					   {"nRCJetsM100","nRCJetsM100"},
					   {"Sum$(rcjet_d12)","Sum__rcjet_d12_"},
					   {"Sum$(rcjet_d23)","Sum__rcjet_d23_"},
					   {"mtw","mtw"},
					   {"met_met","met_met"},
					   {"Sum$(jet_pcb_MV2c10_btag_ordered*(Iteration$<6))","Sum__jet_pcb_MV2c10_btag_ordered__Iteration_s6__"}
  };

  vector<double> mass = {400,500,600,700,800,900,1000};
 
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

  TCanvas *c_mp = new TCanvas("c_mp");
  c_mp->SetTickx();c_mp->SetTicky();
  double cs_bkg = getHist(t_test_1,"cs_bkg1","1","(classID&&"+cut+")*weight",10,0,0)->GetMaximum()+
    getHist(t_test_2,"cs_bkg2","1","(classID&&"+cut+")*weight",10,0,0)->GetMaximum();
  double cs_sig = getHist(t_test_1,"cs_sig1","1","(!classID&&"+cut+")*weight",10,0,0)->GetMaximum()+
    getHist(t_test_2,"cs_sig2","1","(!classID&&"+cut+")*weight",10,0,0)->GetMaximum();
  double cn_bkg = t_test_1->GetEntries(("(classID&&"+cut+")").c_str())+t_test_2->GetEntries(("(classID&&"+cut+")").c_str());
  double cn_sig = t_test_1->GetEntries(("(!classID&&"+cut+")").c_str())+t_test_2->GetEntries(("(!classID&&"+cut+")").c_str());

  cout << "\nTotal Sig: " << cn_sig
       << "  Total Bkg: " << cn_bkg
       << "\n" << endl;

  cout << "\nTotal Weighted Sig: " << cs_sig
       << "  Total Weighted Bkg: " << cs_bkg
       << "\n" << endl;

  TH1F *h_mp_sig = getHist(t_test_1,"h_mp_sig1","ttH_tttt_m","(!classID&&"+cut+")*weight/"+to_string(cs_sig),80,300,1100);
  h_mp_sig->Add(getHist(t_test_2,"h_mp_sig2","ttH_tttt_m","(!classID&&"+cut+")*weight/"+to_string(cs_sig),80,300,1100));
  TH1F *h_mp_bkg = getHist(t_test_1,"h_mp_bkg1","ttH_tttt_m","(classID&&"+cut+")*weight/"+to_string(cs_bkg),80,300,1100);
  h_mp_bkg->Add(getHist(t_test_2,"h_mp_bkg2","ttH_tttt_m","(classID&&"+cut+")*weight/"+to_string(cs_bkg),80,300,1100));
  h_mp_sig->SetLineWidth(3);h_mp_sig->SetLineColor(kOrange+1);h_mp_sig->SetLineStyle(1);
  h_mp_bkg->SetLineWidth(3);h_mp_bkg->SetLineColor(kAzure-3);h_mp_bkg->SetLineStyle(9);
  h_mp_sig->GetXaxis()->SetTitle("Mass");
  TH1F *h_mpc_sig = getHist(t_test_1,"h_mpc_sig1","ttH_tttt_m","(!classID&&"+cut+")*1.0/"+to_string(cn_sig),80,300,1100);
  h_mpc_sig->Add(getHist(t_test_2,"h_mpc_sig2","ttH_tttt_m","(!classID&&"+cut+")*1.0/"+to_string(cn_sig),80,300,1100));
  TH1F *h_mpc_bkg = getHist(t_test_1,"h_mpc_bkg1","ttH_tttt_m","(classID&&"+cut+")*1.0/"+to_string(cn_bkg),80,300,1100);
  h_mpc_bkg->Add(getHist(t_test_2,"h_mpc_bkg2","ttH_tttt_m","(classID&&"+cut+")*1.0/"+to_string(cn_bkg),80,300,1100));
  h_mpc_sig->SetLineWidth(3);h_mpc_sig->SetLineColor(kOrange+1);h_mpc_sig->SetMarkerColor(kOrange+1);h_mpc_sig->SetMarkerStyle(21);h_mpc_sig->SetMarkerSize(2);
  h_mpc_bkg->SetLineWidth(3);h_mpc_bkg->SetLineColor(kAzure-3);h_mpc_bkg->SetMarkerColor(kAzure-3);h_mpc_bkg->SetMarkerStyle(20);h_mpc_bkg->SetMarkerSize(2);
  h_mpc_sig->GetXaxis()->SetTitle("Mass");
  h_mp_sig->Draw("hist");
  h_mp_bkg->Draw("hist same");
  h_mpc_sig->Draw("e1 same");
  h_mpc_bkg->Draw("e1 same");
  TLegend *l_mp = new TLegend(0.681145,0.746269,0.994077,0.922886);
  l_mp->SetLineWidth(3);
  l_mp->SetFillStyle(0);
  l_mp->AddEntry(h_mpc_sig,("Signal (count: "+to_string((int)cn_sig)+")").c_str(),"e1p");
  l_mp->AddEntry(h_mp_sig,("Signal (weighted: "+to_string(cs_sig)+")").c_str(),"l");
  l_mp->AddEntry(h_mpc_bkg,("Background (count: "+to_string((int)cn_bkg)+")").c_str(),"e1p");
  l_mp->AddEntry(h_mp_bkg,("Background (weighted: "+to_string(cs_bkg)+")").c_str(),"l");
  l_mp->Draw();
  c_mp->SaveAs("plots/variables/mp_plot.png");

  
  for (auto var : variables){
    TCanvas *cbkg = new TCanvas(("cbkg"+var.second).c_str());
    cbkg->SetTickx();cbkg->SetTicky();
    string cut_exp_bkg = "(classID&&"+cut+")*1.0";
    TH1F *h_bkg = getHist(t_test_1,"hbkg_0"+var.first,var.second,cut_exp_bkg,80,0,0);
    double xmin = h_bkg->GetXaxis()->GetXmin();
    double xmax = h_bkg->GetXaxis()->GetXmax();
    TH1F *h1 = getHist(tr_bkg,"h1_bkg"+var.second,var.first,cut+"*1.0",80,xmin,xmax);
    TH1F *h2 = getHist(t_test_1,"h2_bkg"+var.first,var.second,cut_exp_bkg,80,xmin,xmax);
    h2->Add(getHist(t_test_2,"h2_bkg_add"+var.first,var.second,cut_exp_bkg,80,xmin,xmax));
    h1->SetLineWidth(3);h1->SetLineColor(kGray+3);h1->SetLineStyle(1);
    h2->SetLineWidth(3);h2->SetLineColor(kRed-4);h2->SetLineStyle(2);
    h1->GetXaxis()->SetTitle(var.second.c_str());
    h1->Draw("hist");
    h2->Draw("hist same");
    TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
    l->SetLineWidth(3);
    l->SetFillStyle(0);
    l->AddEntry(h1,"Original","l");
    l->AddEntry(h2,"TMVA","l");
    l->Draw();
    cbkg->SaveAs(("plots/variables/bkg_or/"+var.second+"bkg_comp_or.png").c_str());
  }
    

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

    for (auto var : variables){
      TCanvas *csig = new TCanvas(("csig"+var.second+mp).c_str());
      csig->SetTickx();csig->SetTicky();
      string cut_exp_sig = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*1.0";
      TH1F *h_sig = getHist(t_test_1,"hsig_0"+mp+var.first,var.second,cut_exp_sig,80,0,0);
      double xmin = h_sig->GetXaxis()->GetXmin();
      double xmax = h_sig->GetXaxis()->GetXmax();
      TH1F *h1 = getHist(tr_sig,"h1"+mp+var.second,var.first,cut+"*1.0",80,xmin,xmax);
      TH1F *h2 = getHist(t_test_1,"h2"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax);
      h2->Add(getHist(t_test_2,"h2_add"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax));
      h1->SetLineWidth(3);h1->SetLineColor(kGray+3);h1->SetLineStyle(1);
      h2->SetLineWidth(3);h2->SetLineColor(kRed-4);h2->SetLineStyle(2);
      h1->GetXaxis()->SetTitle(var.second.c_str());
      h1->Draw("hist");
      h2->Draw("hist same");
      TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
      l->SetLineWidth(3);
      l->SetFillStyle(0);
      l->SetHeader(("Mass: "+mp).c_str());
      l->AddEntry(h1,"Original","l");
      l->AddEntry(h2,"TMVA","l");
      l->Draw();
      csig->SaveAs(("plots/variables/sig_or/"+var.second+mp+"sig_comp_or.png").c_str());
    }
    
  }

  for (auto var : variables){
    TCanvas *csig_test = new TCanvas(("csig_test"+var.second).c_str());
    csig_test->SetTickx();csig_test->SetTicky();
    csig_test->SetLogy();
    TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
    l->SetLineWidth(3);
    l->SetFillStyle(0);
    string cut_exp_sig0 = "(!classID&&"+cut+")*weight";
    TH1F *h_sig_test = getHist(t_test_1,"hsig_test0"+var.first,var.second,cut_exp_sig0,80,0,0);
    double xmin = h_sig_test->GetXaxis()->GetXmin();
    double xmax = h_sig_test->GetXaxis()->GetXmax();
    for (auto mp : MP){
      string cut_exp_sig = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *h = getHist(t_test_1,"hsig_test1"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax);
      h->Add(getHist(t_test_2,"hsig_test2"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax));
      h->SetLineWidth(3);h->SetLineColor(34+stoi(mp.c_str())/100);h->SetLineStyle(1);
      if (mp == "400"){
	double ymax = -1e32;
	for (int i=0;i<80;i++)
	  if (ymax<h->GetBinContent(i+1))
	    ymax = h->GetBinContent(i+1);
	h->SetMaximum(3.0*ymax);
	h->GetXaxis()->SetTitle(var.second.c_str());
	h->Draw("hist");
      }
      else{
	h->Draw("hist same");
      }
      l->AddEntry(h,mp.c_str(),"l");
    }
    l->Draw();
    csig_test->SaveAs(("plots/variables/sig_test/"+var.second+"sig_mp.png").c_str());
  }
  
  for (auto var : variables){
    TCanvas *csig_train = new TCanvas(("csig_train"+var.second).c_str());
    csig_train->SetTickx();csig_train->SetTicky();
    csig_train->SetLogy();
    TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
    l->SetLineWidth(3);
    l->SetFillStyle(0);
    string cut_exp_sig0 = "(!classID&&"+cut+")*weight";
    TH1F *h_sig_train = getHist(t_train_1,"hsig_train0"+var.first,var.second,cut_exp_sig0,80,0,0);
    double xmin = h_sig_train->GetXaxis()->GetXmin();
    double xmax = h_sig_train->GetXaxis()->GetXmax();
    for (auto mp : MP){
      string cut_exp_sig = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *h = getHist(t_train_1,"hsig_train1"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax);
      h->Add(getHist(t_train_2,"hsig_train2"+mp+var.first,var.second,cut_exp_sig,80,xmin,xmax));
      h->SetLineWidth(3);h->SetLineColor(34+stoi(mp.c_str())/100);h->SetLineStyle(1);
      if (mp == "400"){
	double ymax = -1e32;
	for (int i=0;i<80;i++)
	  if (ymax<h->GetBinContent(i+1))
	    ymax = h->GetBinContent(i+1);
	h->SetMaximum(3.0*ymax);
	h->GetXaxis()->SetTitle(var.second.c_str());
	h->Draw("hist");
      }
      else{
	h->Draw("hist same");
      }
      l->AddEntry(h,mp.c_str(),"l");
    }
    l->Draw();
    csig_train->SaveAs(("plots/variables/sig_train/"+var.second+"sig_mp.png").c_str());
  }
  
  for (auto var : variables){
    TCanvas *cbkg_test = new TCanvas(("cbkg_test"+var.second).c_str());
    cbkg_test->SetLogy();
    cbkg_test->SetTickx();cbkg_test->SetTicky();
    TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
    l->SetLineWidth(3);
    l->SetFillStyle(0);
    string cut_exp_bkg0 = "(classID&&"+cut+")*weight";
    TH1F *h_bkg_test = getHist(t_test_1,"hbkg_test0"+var.first,var.second,cut_exp_bkg0,80,0,0);
    double xmin = h_bkg_test->GetXaxis()->GetXmin();
    double xmax = h_bkg_test->GetXaxis()->GetXmax();
    for (auto mp : MP){
      string cut_exp_bkg = "(classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *h = getHist(t_test_1,"hbkg_test1"+mp+var.first,var.second,cut_exp_bkg,80,xmin,xmax);
      h->Add(getHist(t_test_2,"hbkg_test2"+mp+var.first,var.second,cut_exp_bkg,80,xmin,xmax));
      h->SetLineWidth(3);h->SetLineColor(34+stoi(mp.c_str())/100);h->SetLineStyle(1);
      if (mp == "400"){
	double ymax = -1e32;
	for (int i=0;i<80;i++)
	  if (ymax<h->GetBinContent(i+1))
	    ymax = h->GetBinContent(i+1);
	h->SetMaximum(3.0*ymax);
	h->GetXaxis()->SetTitle(var.second.c_str());
	h->Draw("hist");
      }
      else{
	h->Draw("hist same");
      }
      l->AddEntry(h,mp.c_str(),"l");
    }
    l->Draw();
    cbkg_test->SaveAs(("plots/variables/bkg_test/"+var.second+"bkg_mp.png").c_str());
  }
  
  for (auto var : variables){
    TCanvas *cbkg_train = new TCanvas(("cbkg_train"+var.second).c_str());
    cbkg_train->SetLogy();
    cbkg_train->SetTickx();cbkg_train->SetTicky();
    TLegend *l = new TLegend(0.681145,0.746269,0.994077,0.922886);
    l->SetLineWidth(3);
    l->SetFillStyle(0);
    string cut_exp_bkg0 = "(classID&&"+cut+")*weight";
    TH1F *h_bkg_train = getHist(t_train_1,"hbkg_train0"+var.first,var.second,cut_exp_bkg0,80,0,0);
    double xmin = h_bkg_train->GetXaxis()->GetXmin();
    double xmax = h_bkg_train->GetXaxis()->GetXmax();
    for (auto mp : MP){
      string cut_exp_bkg = "(classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *h = getHist(t_train_1,"hbkg_train1"+mp+var.first,var.second,cut_exp_bkg,80,xmin,xmax);
      h->Add(getHist(t_train_2,"hbkg_train2"+mp+var.first,var.second,cut_exp_bkg,80,xmin,xmax));
      h->SetLineWidth(3);h->SetLineColor(34+stoi(mp.c_str())/100);h->SetLineStyle(1);
      if (mp == "400"){
	double ymax = -1e32;
	for (int i=0;i<80;i++)
	  if (ymax<h->GetBinContent(i+1))
	    ymax = h->GetBinContent(i+1);
	h->SetMaximum(3.0*ymax);
	h->GetXaxis()->SetTitle(var.second.c_str());
	h->Draw("hist");
      }
      else{
	h->Draw("hist same");
      }
      l->AddEntry(h,mp.c_str(),"l");
    }
    l->Draw();
    cbkg_train->SaveAs(("plots/variables/bkg_train/"+var.second+"bkg_mp.png").c_str());
  }

  
  for (auto mp : MP){
    for (auto var : variables){
      TCanvas *csplit1 = new TCanvas(("csplit1"+var.second+mp).c_str());
      csplit1->SetTickx();csplit1->SetTicky();
      string cut_exp_sig_split1 = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *hsig_test = getHist(t_test_1,"hsig_test_split1"+mp+var.second,var.second,cut_exp_sig_split1,80,0,0);
      double xmin = hsig_test->GetXaxis()->GetXmin();
      double xmax = hsig_test->GetXaxis()->GetXmax();
      TH1F *hsig_train = getHist(t_train_1,"hsig_train_split1"+mp+var.second,var.second,cut_exp_sig_split1,80,xmin,xmax);
      string cut_exp_bkg_split1 = "(classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *hbkg_test = getHist(t_test_1,"hbkg_test_split1"+mp+var.second,var.second,cut_exp_bkg_split1,80,xmin,xmax);
      TH1F *hbkg_train = getHist(t_train_1,"hbkg_train_split1"+mp+var.second,var.second,cut_exp_bkg_split1,80,xmin,xmax);
      draw_train_test(hsig_train,hbkg_train,hsig_test,hbkg_test,mp,var.second,"split1");
      csplit1->SaveAs(("plots/variables/split1/"+var.second+mp+"split.png").c_str());
    }
    
  }
  
  for (auto mp : MP){
    for (auto var : variables){
      TCanvas *csplit2 = new TCanvas(("csplit2"+var.second+mp).c_str());
      csplit2->SetTickx();csplit2->SetTicky();
      string cut_exp_sig_split2 = "(!classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *hsig_test = getHist(t_test_2,"hsig_test_split2"+mp+var.second,var.second,cut_exp_sig_split2,80,0,0);
      double xmin = hsig_test->GetXaxis()->GetXmin();
      double xmax = hsig_test->GetXaxis()->GetXmax();
      TH1F *hsig_train = getHist(t_train_2,"hsig_train_split2"+mp+var.second,var.second,cut_exp_sig_split2,80,xmin,xmax);
      string cut_exp_bkg_split2 = "(classID&&ttH_tttt_m=="+mp+"&&"+cut+")*weight";
      TH1F *hbkg_test = getHist(t_test_2,"hbkg_test_split2"+mp+var.second,var.second,cut_exp_bkg_split2,80,xmin,xmax);
      TH1F *hbkg_train = getHist(t_train_2,"hbkg_train_split2"+mp+var.second,var.second,cut_exp_bkg_split2,80,xmin,xmax);
      draw_train_test(hsig_train,hbkg_train,hsig_test,hbkg_test,mp,var.second,"split2");
      csplit2->SaveAs(("plots/variables/split2/"+var.second+mp+"split.png").c_str());
    }
    
  }
  
}
