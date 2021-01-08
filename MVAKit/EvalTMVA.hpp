#ifndef EVALTMVA_HPP
#define EVALTMVA_HPP

#include "MVAKit/MVAKit.hpp"

using namespace std;


class EvalTMVA {

public:

  EvalTMVA(string xml,string cnf);
  ~EvalTMVA();

  void Init();
  void Reset();

  Double_t GetScore(vector<Double_t> x={0}, int en=0);
  Double_t GetMultiClassScore(vector<Double_t> x={0}, int en=0, int cl=0);
  Double_t GetScoreRatio(vector<Double_t> x={0}, int en=0, int cl1=0, int cl2=1);

private:

  vector<pair<int,int>> GetReverseCond(vector<string> cond);
    
  vector<TMVA::Reader*> m_tmva_reader;
  vector<Float_t*> m_vec_variables;
  vector<Float_t*> m_vec_variablesSpec;
  vector<pair<int,int>> m_reverse_cond;
  TString m_method;
  string m_xml_file;
  string m_cnf_file;
  
  unique_ptr<MVAKit> m_tool;
};

#endif
