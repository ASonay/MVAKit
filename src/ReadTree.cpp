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
ReadTree::SetVariables(const string title,vector<pair<string,string>> vars)
{
  for (auto const &var : vars){
    m_vars[title].push_back(var.second);
  }
}

void
ReadTree::SetVariables(const string title,map<string,string> vars)
{
  for (auto const &var : vars){
    m_vars[title].push_back(var.second);
  }
}

void
ReadTree::SetVariables(const string title,const vector<string> vars)
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
ReadTree::SetVariables(const string title,const string var)
{
  m_vars[title].push_back(var);
}

void
ReadTree::ResetVariables()
{
  for (auto &x : m_vars) x.second.clear();
  m_vars.clear();
}

void
ReadTree::ProcessVariables()
{
  int index=1;
  string vars;
  for (auto const& x : m_vars){
    for (auto var : x.second){
      string vname = x.first+to_string(index);
      m_alias.push_back(make_pair(vname,var));
      vars += ":"+vname;
      m_label_index[x.first].push_back(index);
      index++;
    }
  }
  m_var_exp = "w0"+vars;
}


void
ReadTree::SetFormulas(const string fname, const string tname, const string cut, const string weight)
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
  
  for (auto x : m_alias){
    m_chain->SetAlias(x.first.c_str(),x.second.c_str());
    m_formula[x.first].reset(new TTreeFormula(("formula_"+x.first).c_str(),x.first.c_str(),m_chain.get()));
    m_formula[x.first]->SetQuickLoad(kTRUE);
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

void
ReadTree::SetInputs(const string fname, const string tname, const string cut, const string weight)
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
  
  for (auto x : m_alias){
    m_chain->SetAlias(x.first.c_str(),x.second.c_str());
  }
  string weight_new(weight);
  if (weight_new == "1") weight_new = "abs(1)";
  m_chain->SetAlias("w0",weight_new.c_str());

  if (!m_var_exp.empty())
    {m_entries = m_chain->Draw(m_var_exp.c_str(),cut.c_str(),"goff");}
  else {
    cout << "You should use ->ProcessVariables()" << endl;
    exit(0);
  }
  
  if (m_entries > 0){
    for (auto const& x : m_label_index){
      for (auto i : x.second) {
	m_data[i]=m_chain->GetVal(i);
      }
    }
  }
  else {
    cout << "One of your variable is not match with any leaf in your tree." << endl;
    for (auto x : m_alias) cout << x.second << endl;
    exit(0);
  }
  
  m_weight=m_chain->GetVal(0);

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
ReadTree::GetInputs(const string label, int index){
  vector<double> vec;

  for (auto i : m_label_index[label]){
    vec.push_back(m_data[i][index]); 
  }
  
  return vec;
}

vector<double>
ReadTree::GetInputs(const string label){
  vector<double> vec;

  for (auto i : m_label_index[label]){
    string key = label+to_string(i);
    m_formula[key]->GetNdata();
    vec.push_back(m_formula[key]->EvalInstance(0));
  }
  
  return vec;
}

double
ReadTree::GetWeight(){
  m_formula_weight->GetNdata();
  return m_formula_weight->EvalInstance(0);
}

vector<string>
ReadTree::CheckVars(const string cut, const string weight)
{
  vector<string> trVars;
  vector<string> activeVars;
  
  for (auto x : *m_chain->GetListOfLeaves()) trVars.push_back(x->GetName());
  for (auto const& vars : m_vars){
    for (auto x : vars.second){
      if (!isVariableExist(x,trVars,activeVars)){
	cout << x << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }
  if (!isVariableExist(cut,trVars,activeVars)){
    cout << cut << " is not include any of the leaf in the tree." << endl;
    exit(0);
  }
  if (!isVariableExist(weight,trVars,activeVars)){
    cout << weight << " is not include any of the leaf in the tree." << endl;
    exit(0);
  }

  return activeVars;
}

bool
ReadTree::isVariableExist(const string var, const vector<string> trVars, vector<string> &activeVars)
{
  bool check=false;
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
