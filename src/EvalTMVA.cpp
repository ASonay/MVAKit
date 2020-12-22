#include "TMVA/Reader.h"
#include "TMVA/PyMethodBase.h"

#include "MVAKit/EvalTMVA.hpp"

using namespace std;



EvalTMVA::EvalTMVA(string xml,string cnf)
{
  
  m_xml_file = xml;
  m_cnf_file = cnf;

  cout << "XML file will be readed: " << m_xml_file << endl;
  cout << "Config file will be readed: " << m_cnf_file << endl;
}

EvalTMVA::~EvalTMVA()
{}

void EvalTMVA::Init()
{
  // Configuration part-----------------------------
  m_tool = unique_ptr<MVAKit>(new MVAKit("Evaluate"));

  m_tool->isClassification(0);
  m_tool->isExe(0);
  m_tool->SetConf(m_cnf_file);
  m_tool->SetXML(m_xml_file);
  m_tool->ReadConf();
  
  TMVA::PyMethodBase::PyInitialize();
 
  vector<string> split = m_tool->GetSplit();m_reverse_cond=GetReverseCond(split);
  vector<TString> variables = m_tool->GetTVariables();
  vector<TString> variablesSpec = m_tool->GetTVariablesOther();
  vector<TString> weight_file = m_tool->GetTXML();
  const int variables_size = variables.size();
  const int variablesSpec_size = variablesSpec.size();
  // Configuration part-----------------------------
  //Book TMVA---------------------------------------
  m_method = m_tool->GetClassifierOpt()+" method";
  m_tmva_reader->reserve(weight_file.size());
  for (auto wf : weight_file){
    auto rdr= make_unique<TMVA::Reader>( "!Color:!Silent" );
    Float_t *local_variables = new Float_t[variables_size];
    Float_t *local_variablesSpec = new Float_t[variablesSpec_size];
    for(int i=0;i<variables_size;i++)
      rdr->AddVariable(string(variables[i]).c_str() , &local_variables[i] );
    for(int i=0;i<variablesSpec_size;i++)
      rdr->AddSpectator(string(variablesSpec[i]).c_str() , &local_variablesSpec[i] );
    m_vec_variables.push_back(local_variables);
    m_vec_variablesSpec.push_back(local_variablesSpec);
    rdr->BookMVA( m_method, wf );
    m_tmva_reader->push_back(move(rdr));
  }
}

vector<pair<int,int>> EvalTMVA::GetReverseCond(vector<string> cond)
{
  vector<pair<int,int>> x;
  for (auto c : cond){
    string first=c.substr(c.find("%") + 1);
    string second=c.substr(c.find("=") + 2);
    int a=atoi(first.c_str());
    int b=atoi(second.c_str());
    x.push_back(make_pair(a,b));
  }
  reverse(x.begin(),x.end());
  cout << "\nYour Selections:" << endl;
  for (auto v : x){
    cout << "<var>%" << v.first << "==" << v.second << endl;
  }
  return x;
}

Double_t EvalTMVA::GetScore(vector<Double_t> x, int en)
{
  Double_t score = -999.;

  for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
    for (unsigned i=0;i<x.size();i++)
      {m_vec_variables[rsize][i]=x[i];}
  }

  if (m_tmva_reader->size()>1){
    for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
      if (en%m_reverse_cond[rsize].first==m_reverse_cond[rsize].second)
	score=m_tmva_reader->at(rsize)->EvaluateMVA(m_method);
    }
  }
  else{
    score=m_tmva_reader->at(0)->EvaluateMVA(m_method);
  }

  return score;
}

Double_t EvalTMVA::GetMultiClassScore(vector<Double_t> x, int en, int cl)
{
  Double_t score = -999.;

  for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
    for (unsigned i=0;i<x.size();i++)
      {m_vec_variables[rsize][i]=x[i];}
  }

  if (m_tmva_reader->size()>1){
    for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
      if (en%m_reverse_cond[rsize].first==m_reverse_cond[rsize].second)
	score=m_tmva_reader->at(rsize)->EvaluateMulticlass(m_method)[cl];
    }
  }
  else{
    score=m_tmva_reader->at(0)->EvaluateMulticlass(m_method)[cl];
  }

  return score;
}

Double_t EvalTMVA::GetScoreRatio(vector<Double_t> x, int en, int cl1, int cl2)
{
  
  Double_t score = -999.;

  for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
    for (unsigned i=0;i<x.size();i++)
      {m_vec_variables[rsize][i]=x[i];}
  }

  if (m_tmva_reader->size()>1){
    for (unsigned rsize=0;rsize<m_tmva_reader->size();rsize++){
      if (en%m_reverse_cond[rsize].first==m_reverse_cond[rsize].second)
	score=m_tmva_reader->at(rsize)->EvaluateMulticlass(m_method)[cl1]/m_tmva_reader->at(0)->EvaluateMulticlass(m_method)[cl2];
    }
  }
  else{
    score=m_tmva_reader->at(0)->EvaluateMulticlass(m_method)[cl1]/m_tmva_reader->at(0)->EvaluateMulticlass(m_method)[cl2];
  }
  return score;
 
}
