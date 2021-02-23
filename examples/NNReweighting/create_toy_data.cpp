#include "TTree.h"

void create_toy_data()
{

  TF1 *f_data = new TF1("f_data","[0]*exp([1]*x)",100,1000);
  TF1 *f_mc = new TF1("f_mc","[0]*exp([1]*x)",100,1000);

  f_data->SetParameters(1200,-0.005);
  f_mc->SetParameters(800,-0.003);

  float x,w;
  
  TFile *fil_data = new TFile("data.root","recreate");
  TTree *tr_data = new TTree("tree","data");
  tr_data->Branch("x",&x,"x/F");

  for (int i=0;i<10000;i++){
    x=f_data->GetRandom(100,1000);
    tr_data->Fill();
  }
  tr_data->Write();
  fil_data->Write();
  fil_data->Close();

  TFile *fil_mc = new TFile("mc.root","recreate");
  TTree *tr_mc = new TTree("tree","mc");
  tr_mc->Branch("x",&x,"x/F");
  tr_mc->Branch("weight",&w,"weight/F");

  for (int i=0;i<100000;i++){
    x=f_mc->GetRandom(100,1000);
    w=1./10.;
    tr_mc->Fill();
  }
  tr_mc->Write();
  fil_mc->Write();
  fil_mc->Close();

  TCanvas *c = new TCanvas("c");
  
  f_data->Draw();
  f_mc->Draw("same");

  c->SaveAs("plots/toy_data.png");
  
}
