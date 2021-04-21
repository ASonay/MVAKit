#include <iostream>
#include <vector>
#include <tuple>
#include "TLorentzVector.h"

struct kin
{
  Double_t pt;
  Double_t eta;
  Double_t phi;
  Double_t e;
};

static vector<kin> jets;
static int instance = 0;

Double_t calc_Mjjj_dRs3Avg() {

  // CALCULATE
  TLorentzVector tl_vec,tl_jet1,tl_jet2,tl_jet3;
  
  unsigned int jets_n = jets.size();
  Double_t mass=0.0;
  unsigned int n_mass=0;
  
  for(unsigned int i = 0; i < jets_n; i++){
    for(unsigned int k = 0; k < jets_n; k++){
      for(unsigned int h = 0; h < jets_n; h++){
	if(i != k && i!=h && h != k){
	    
	  tl_jet1.SetPtEtaPhiE(jets.at(i).pt,jets.at(i).eta,jets.at(i).phi,jets.at(i).e);
	  tl_jet2.SetPtEtaPhiE(jets.at(k).pt,jets.at(k).eta,jets.at(k).phi,jets.at(k).e);
	  tl_jet3.SetPtEtaPhiE(jets.at(h).pt,jets.at(h).eta,jets.at(h).phi,jets.at(h).e);
	    
	  tl_vec =tl_jet1 + tl_jet2 + tl_jet3;
	  auto current_trijet_mass = tl_vec.M();
	  auto current_deltaR = sqrt(pow(tl_jet1.DeltaR(tl_jet2),2) + pow(tl_jet1.DeltaR(tl_jet3),2) + pow(tl_jet2.DeltaR(tl_jet3),2));
	    
	  if(current_deltaR < 3){
	    mass += current_trijet_mass;
	    n_mass++;
	  }
	    
	}
      }
    }
  }

  //std::cout << "mass : " << mass << " n: " << n_mass << std::endl; 
  
  return mass/static_cast<Double_t>(n_mass>0?n_mass:1);
}

Double_t calc_Mjjj_dRs3Avg(Double_t pt, Double_t eta, Double_t phi, Double_t e, int n) {
  /*
  std::cout << "NJets: " << n << " instance: " << instance << " | "
	    << " pt: " << pt
	    << " eta: " << eta
	    << " phi: " << phi
	    << " e: " << e
	    << std::endl; 
  */
  
  if (instance == 0) {
    jets.clear();
  }
  
  kin k = {pt,eta,phi,e};
  jets.push_back(k);
  instance++;

  if (instance == n)
    {instance=0;return calc_Mjjj_dRs3Avg();}
  else
    {return -100;}
}


bool cumulate_jets(Double_t pt, Double_t eta, Double_t phi, Double_t e, int n) {
  /*
  std::cout << "NJets: " << n << " instance: " << instance << " | "
	    << " pt: " << pt
	    << " eta: " << eta
	    << " phi: " << phi
	    << " e: " << e
	    << std::endl; 
  */
  if (instance == 0) {
    jets.clear();
  }
  
  kin k = {pt,eta,phi,e};
  jets.push_back(k);
  instance++;

  if (instance == n)
    {instance=0;return true;}
  else
    {return false;}

}
