#include <iomanip>

#include "MVAKit/ReadTree.hpp"

using namespace std;

ReadTree::ReadTree():
  m_entry(0)
{}


ReadTree::~ReadTree()
{
  m_formula.clear();
}

void
ReadTree::SetAlias(const vector<pair<string,string>> &alias)
{
  m_alias_by_request = alias;
}

void
ReadTree::SetVariables(const string &title, const vector<VariableStore> &vars)
{
  for (auto const &var : vars){
    m_vars[title].push_back(var.first);
  }
}

void
ReadTree::SetVariables(const string &title,const map<string,string> &vars)
{
  for (auto const &var : vars){
    m_vars[title].push_back(var.first);
  }
}

void
ReadTree::SetVariables(const string &title, const vector<string> &vars)
{
  if (m_vars[title].size()==0){
    m_vars[title] = vars;
  }
  else {
    cout << "\nAnother sets of variables will be added for " << title << endl;
    for (auto var : vars) m_vars[title].push_back(var);
  } 
}

void
ReadTree::SetVariables(const string &title, const string &var)
{
  m_vars[title].push_back(var);
}

void
ReadTree::ResetVariables()
{
  for (auto &vars : m_vars) vars.second.clear();
  m_vars.clear();
}

void
ReadTree::ProcessVariables()
{
  unsigned index=1, index_var=1;
  string variables;
  for (auto const& vars : m_vars){
    map<unsigned,vector<pair<string,string>>> map_tmp;
    for (auto var : vars.second){
      for (auto x : Common::StringSep(var,';')) {
	string vname = vars.first+to_string(index_var);
	map_tmp[index].push_back(make_pair(vname,x));
	index_var++;
      }
      index++;
    }
    m_alias[vars.first] = map_tmp;
  }
}


void
ReadTree::SetFormulas(const string &fname, const string &tname, const string &cut, const string &weight)
{
  m_chain.reset(new TChain(tname.c_str()));

  if (m_chain->Add(fname.c_str())==1){
    cout << "Successfully added your file to the chain." << endl;
  }
  else {
    cout << "Can not find " << fname << endl;
    exit(0);
  }

  long long entries = m_chain->GetEntries();
  m_chain->SetEstimate(entries);

  vector<string> activeVars = CheckVars(cut,weight);
  m_chain->SetBranchStatus("*",0);
  for (auto av : activeVars) m_chain->SetBranchStatus(av.c_str(),1);
  
  for (auto x : m_alias_by_request){
    m_chain->SetAlias(x.first.c_str(),x.second.c_str());
  }
  
  for (const auto &labels : m_alias){
    for (const auto &index : labels.second) {
      for (auto x : index.second) {
	m_chain->SetAlias(x.first.c_str(),x.second.c_str());
	m_formula[x.first].reset(new TTreeFormula(("formula_"+x.first).c_str(),x.first.c_str(),m_chain.get()));
	m_formula[x.first]->SetQuickLoad(kTRUE);
      }
    }
  }
  
  string weight_new(weight);
  if (weight_new == "1") {weight_new = "abs(1)";}
  m_chain->SetAlias("w0",weight_new.c_str());
  m_formula_weight.reset(new TTreeFormula("formula_w0","w0",m_chain.get()));
  m_formula_weight->SetQuickLoad(kTRUE);
  
  string cut_new(cut);
  if (cut_new == "1") {cut_new = "abs(1)";}
  m_chain->SetAlias("c0",cut_new.c_str());
  m_formula_cut.reset(new TTreeFormula("formula_c0","c0",m_chain.get()));
  m_formula_weight->SetQuickLoad(kTRUE);

  m_entry=0;
  m_entries=m_chain->GetEntries();
}

bool
ReadTree::NextEvent(){

  for (long long i=m_entry;i<m_entries;i++){
    m_chain->GetEntry(i);
    m_formula_cut->GetNdata();
    if (!(int)m_formula_cut->EvalInstance(0)) {continue;}
    else{
      m_entry=i+1;
      return true;
    }
  }
  
  m_entry=0;
  return false;
}

vector<double>
ReadTree::GetInputs(const string &label){
  vector<double> vec;

  for (const auto &i : m_alias[label]){
    for (const auto & alias : i.second){
      string key = alias.first;
      m_formula[key]->GetNdata();
      vec.push_back(m_formula[key]->EvalInstance(0));
    }
  }
  return vec;
}

vector<vector<double>>
ReadTree::GetVectors(const string &label){
  vector<vector<double>> vec2d;

  for (const auto &i : m_alias[label]){
    vector<double> vec;
    for (const auto & alias : i.second){
      string key = alias.first;
      unsigned n_f = m_formula[key]->GetNdata();
      for (unsigned j=0;j<n_f;j++){
	vec.push_back(m_formula[key]->EvalInstance(j));
      }
    }
    vec2d.push_back(vec);
  }
  return vec2d;
}

double
ReadTree::GetWeight(){
  m_formula_weight->GetNdata();
  return m_formula_weight->EvalInstance(0);
}

vector<string>
ReadTree::CheckVars(const string &cut, const string &weight)
{
  vector<string> trVars;
  vector<string> activeVars;
  
  for (auto x : *m_chain->GetListOfLeaves()) trVars.push_back(x->GetName());
  for (auto const& vars : m_vars){
    for (auto var : vars.second){
      for (auto x :  Common::StringSep(var,';')){
	if (!isVariableExist(x,trVars,activeVars)){
	  cout << x << " cannot find in the tree." << endl;
	  exit(0);
	}
      }
    }
  }
  for (auto const & vars : m_alias_by_request){
      if (!isVariableExist(vars.second,trVars,activeVars)){
	cout << vars.second << " cannot find in the tree." << endl;
	exit(0);
      }    
  }
  if (!isVariableExist(cut,trVars,activeVars)){
    cout << cut << " cannot find in the tree." << endl;
    exit(0);
  }
  if (!isVariableExist(weight,trVars,activeVars)){
    cout << weight << " cannot find in the tree." << endl;
    exit(0);
  }

  return activeVars;
}

bool
ReadTree::isVariableExist(const string &var, const vector<string> &trVars, vector<string> &activeVars)
{
  bool check=false;
  for (auto const & vars : m_alias_by_request)
    {if (var.find(vars.first) != string::npos) {check=true;}}
  if (var == "1") return true;
  for (auto y : trVars){
    if (var.find(y) != string::npos){
      check=true;
      bool is_filled=false;
      for (auto x : activeVars){
	if (x==y) is_filled=true;
      }
      if (!is_filled) activeVars.push_back(y);
    }
  }

  return check;  
}
