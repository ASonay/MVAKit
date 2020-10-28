#include <thread> 

#include "TSystem.h"

#include "../inc/TMVAConf.hpp"
#include "../inc/TMVARead.hpp"

using namespace std;

TMVARead::TMVARead(TMVAConf &c):
  m_split_per(0),
  m_parameterized(false)
{
  
  m_ntups = c.GetFilesName();
  m_weight = c.GetWeights();
  m_cut = c.GetCuts();
  m_tree = c.GetTrees();
  m_variables = c.GetVariables();
  m_variables_other = c.GetVariablesOther();
  m_split = c.GetSplit();
  m_paramvar = c.GetParameterVariable();
  m_xmlFile = c.GetXML();
  m_cond = c.GetCond();
  m_param = c.GetParameter();
  m_cname = c.GetClassName();
  
  if (m_split.size()==0)
    m_split.push_back("Split:50");

  if (m_split[0].find("Split:") != string::npos)
    {m_split_per = atoi(m_split[0].substr(6).c_str());}

  if (!m_paramvar.empty()){
    cout << "\nParameterization detected for : " << m_paramvar << endl;
    m_parameterized=true;
  }
  
  if (m_parameterized && m_xmlFile.empty()){
    const int columnId = 0;
    const string paramFind = m_paramvar;
    sort(m_ntups.begin(), m_ntups.end(),
	 [&columnId,&paramFind](const pair<string,vector<string>> &a, const pair<string,vector<string>> &b) 
	 {
	   return a.second[columnId].find(paramFind) != string::npos && b.second[columnId].find(paramFind) == string::npos;
	 });
    if (m_ntups[0].second[0].find(m_paramvar) == string::npos){
      cout << "Your parameter : " << m_paramvar << " does not match with any file." << endl;
      exit(0);
    }
  }
  
}

TMVARead::~TMVARead()
{}

void
TMVARead::SetEvents(const vector<TMVA::DataLoader*> &loaders){
  
  if (m_variables.size()<1){
    cout << "Variables has to enter." << endl;
    exit(0);
  }
  
  cout << "\nVariables:" << endl;
  for (auto var : m_variables)
    {cout << var << endl;}
  
  for (auto x : m_ntups)
    {ReadEvents(loaders,x.first,x.second);}
}

void
TMVARead::EvaluateEvents(const vector<TMVA::Reader*> &tmva_reader, const vector<Float_t*> &vec_variables, const vector<Float_t*> &vec_variablesSpec, string name, TString method){
    
  if (m_cond.size()!=m_xmlFile.size()){
    cout << "Number of condition doesn't match with the number of XML files." << endl;
    exit(0);
  }

  if (m_variables.size()<1){
    cout << "Variables has to enter." << endl;
    exit(0);
  }

  string bdt_name = name+"_score";
  double param_val = 0;
  if (!m_cname.empty()) {
    bdt_name = m_cname;
    param_val = atof(m_param.c_str());
  }
  if (m_parameterized){
    bdt_name += "_"+m_paramvar+m_param;
    if (m_param.empty()){
      cout << "You declared ParamVar in the config file, but you did not give any parameter by --par." << endl;
      exit(0);
    }
  }
  
  for (auto ntup : m_ntups){
    string tree_name;
    for (auto tree : m_tree){
      if ( (StringCompare(ntup.first,tree.first)) || tree.first == "All" ) tree_name=tree.second;
    }
    for (auto file : ntup.second){
      TFile *f = new TFile((file).c_str(),"update");
      if (!f) { cout<<"File not found!"<<endl; exit(0); }
      TTree *T; f->GetObject(tree_name.c_str(),T);
      if (!T) { cout<<"Tree not found!"<<endl; delete f; exit(0); }
      cout << "\nUpdating " << tree_name << " for " << file << endl;
      float BDT;
      TBranch *b_BDT = T->Branch(bdt_name.c_str(),&BDT,(bdt_name+"/F").c_str());
      ReadTree read(name,T,m_variables);
      int noe = read.GetNoE();
      for (int i=0;i<noe;i++){
	vector<Double_t> vars = read.GetInput(i);
	if (m_parameterized) vars.push_back(param_val);
	for (auto var : m_variables_other){read.SetSingleVariable(var);vars.push_back(read.GetInputSingle(i));}
	for (unsigned rsize=0;rsize<tmva_reader.size();rsize++){
	  for(unsigned j=0;j<vars.size();j++){
	    vec_variables[rsize][j]=vars[j];
	  }
	}
	for (unsigned rsize=0;rsize<tmva_reader.size();rsize++){
	  read.SetSingleVariable(m_cond[rsize]);
	  if (int(read.GetInputSingle(i)))
	    BDT=tmva_reader[rsize]->EvaluateMVA( method );
	}
	b_BDT->Fill();
	if (i%(noe/100)==0){
	  cout << setw(10) << i << " events has been done over "
	       << setw(10) << noe << " events." << "\r";
	}
      }
      cout << "\nDone. Closing file.." << endl;
      T->Write(tree_name.c_str());
      delete T;
      f->Close("R");
      delete f;
    }
  }
  
}

void
TMVARead::ReadEvents(const vector<TMVA::DataLoader*> &loaders, string label, vector<string> files){

  m_label_current = label;
  
  cout << "\nSampling label : " << m_label_current << " for following samples:\n" << endl;

  map<int,int> tmp_param_map;

  const int split_size = m_split.size();   
  
  unsigned tot_train[split_size]={0},tot_test[split_size]={0};
  float totw_train[split_size]={0},totw_test[split_size]={0};

  if (m_weight.size()==0)
    {m_weight_current="1";}
  else{
    for (auto x : m_weight){
      if (StringCompare(label,x.first)) m_weight_current=x.second;
    }
  }

  if (m_cut.size()==0)
    {m_cut_current="1";}
  else{
    for (auto x : m_cut){
      if (StringCompare(label,x.first)) m_cut_current=x.second;
    }
  }

  string tree_name;
  for (auto x : m_tree){
    if ( (StringCompare(label,x.first)) || x.first == "All" ) tree_name=x.second;
  }

  if (tree_name.empty()){
    cout << "Cannot find any tree name declared in the configuration file or non of them match with the label" << endl;
    exit(0);
  }

  if (m_weight_current.empty()){
    cout << "You defined weight but not matched any of the label!" << endl;
    exit(0);
  }

  if (m_cut_current.empty()){
    cout << "You defined cut but not matched any of the label!" << endl;
    exit(0);
  }

  cout << "CUT EXPRESSION: " << m_cut_current << endl;
  cout << "WEIGHT EXPRESSION: " << m_weight_current << "\n" << endl;

  for (auto x: files){
    cout << "\nFile name to be readed : " << x << "\n" << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      exit(0);
    }
    TTree *tree = (TTree*) f->Get(tree_name.c_str());
    if (!tree) {
      cout << "ERROR: cannot open tree: " << x << endl;
    }
    vector<string> activeVars = CheckVars(tree);
    tree->SetBranchStatus("*",0);
    for (auto av : activeVars) tree->SetBranchStatus(av.c_str(),1);
    for (auto val : TreeSplit(tree->GetEntries())){
      vector<Counter> counter;
      AssignEvents(loaders,tree,x,val,tmp_param_map,counter);
      cout << "Read/Total events : " << val.second << "/" << tree->GetEntries();
      for (int i=0;i<split_size;i++){
	tot_train[i]+=counter[i].count_train;tot_test[i]+=counter[i].count_test;
	totw_train[i]+=counter[i].weight_train;totw_test[i]+=counter[i].weight_test;
	cout << " || Split " << setw(3) << i
	     << " Training/Test events : " << setw(15) << counter[i].count_train
	     << "/" << setw(15) << counter[i].count_test;
      }
      cout << "\r" << flush;
    }
    delete tree;
    f->Close();
    cout << "\n" <<endl;
  }

  cout << m_label_current << " SAMPLE INFO:" << endl;
  for (int i=0;i<split_size;i++){
    cout << "Split " << i << endl;
    cout << "Total training events: " << tot_train[i] << ", weighted: " << totw_train[i] << endl;
    cout << "Total test events:     " << tot_test[i] << ", weighted: " << totw_test[i] << endl;
  }
  
  if (m_parameterized){
    cout << "\nParameterization:" << endl;
    unsigned total_event = tmp_param_map.size();
    vector<double> param_vec,weights;
    for (auto const& x : tmp_param_map)
      {
	double param = x.first;
	double ratio = 1.0*((double)x.second/(double)total_event);
	cout << param
	     << ':' 
	     << x.second
	     << ':'
	     << ratio
	     << endl ;
	  param_vec.push_back(param);
	  weights.push_back(ratio);
      }
    if (m_param_map.size()==0) {
      m_param_map=tmp_param_map;
      m_param_vec=param_vec;
      m_weights=weights;
    }
  }

}

void
TMVARead::AssignEvents(const vector<TMVA::DataLoader*> &loaders, TTree *tr, const string name, pair<int,int> split, map<int,int> &tmp_param_map, vector<Counter> &c)
{
  
  const int split_size = m_split.size(); 
  
  unsigned tmp_train[split_size]={0},tmp_test[split_size]={0};
  float tmpw_train[split_size]={0},tmpw_test[split_size]={0};
    
  random_device rd;
  mt19937 gen(rd());    
  discrete_distribution<int> d(begin(m_weights), end(m_weights));

  TFile *f_tmp = new TFile("f_tmp.root","recreate");
  TTree *tree = GetTree(tr,split.first,split.second);
  ReadTree read(name,tree,m_variables);
  int noe = read.GetNoE();
  for (int i=0;i<noe;i++){
    unsigned cond_index=0;
    for (auto sp : m_split){
      int split_cond=0;
      if (m_split_per==0) {
	read.SetSingleVariable(sp);split_cond=int(read.GetInputSingle(i));
      }
      else {split_cond = int(rand()%100 < m_split_per);}
      read.SetSingleVariable(m_weight_current);
      Double_t weight = read.GetInputSingle(i);
      vector<Double_t> vars = read.GetInput(i);
      if (m_parameterized) vars.push_back(GetParam(name,tmp_param_map,gen,d));
      for (auto var : m_variables_other) {read.SetSingleVariable(var);vars.push_back(read.GetInputSingle(i));}
      if (split_cond){
	loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTraining,vars,weight);
	tmp_train[cond_index]++;
	tmpw_train[cond_index]+=weight;
      }
      else{
	loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTesting,vars,weight);
	tmp_test[cond_index]++;
	tmpw_test[cond_index]+=weight;
      }
      cond_index++;
    }
  }

  for (int i=0;i<split_size;i++) c.push_back({tmp_test[i],tmp_train[i],tmpw_test[i],tmpw_train[i]});
  
  f_tmp->Close();
  if( remove("f_tmp.root") != 0 )
    {cout << "\nError deleting temporary root file." << endl;}
}

TTree*
TMVARead::GetTree(TTree *tree, Long64_t max, Long64_t first)
{
  return tree->CopyTree(m_cut_current.c_str(),"fast",max,first);
}

vector<pair<Long64_t,Long64_t>>
TMVARead::TreeSplit(int noe)
{
  vector<pair<Long64_t,Long64_t>> val;

  int min = 5000;

  if (noe<min){
    return {{noe,0}};
  }
  else{
    for (int i=0;i<noe/min;i++){
      val.push_back(make_pair(min,min*i));
    }
    return val;
  }
}

double
TMVARead::GetParam(string file,map<int,int> &pmap,mt19937 &gen,discrete_distribution<int> &d)
{
  double param=0;

  if (m_param_map.size()==0){
    param = FindDigit(file,m_paramvar);
  }
  else{
    param=m_param_vec[d(gen)];
  }

  pmap[(int)param] += 1;
  
  return param;
}


vector<string>
TMVARead::CheckVars(TTree *tree)
{
  vector<string> trVars;
  vector<string> activeVars;
  
  for (auto x : *tree->GetListOfLeaves()) trVars.push_back(x->GetName());

  for (auto x : m_variables){
    if (!isVariableExist(x,trVars,activeVars)){
      cout << x << " is not include any of the leaf in the tree." << endl;
      exit(0);
    }
  }
  
  for (auto x : m_variables_other){
    if (!isVariableExist(x,trVars,activeVars)){
      cout << x << " is not include any of the leaf in the tree." << endl;
      exit(0);
    }
  }

  if (m_split_per==0){
    for (auto x : m_split){
      if (!isVariableExist(x,trVars,activeVars)){
	cout << x << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }
  
  for (auto x : m_weight){
    if (!CheckDigit(x.second)){
      if (!isVariableExist(x.second,trVars,activeVars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }

  for (auto x : m_cut){
    if (!CheckDigit(x.second)){
      if (!isVariableExist(x.second,trVars,activeVars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }

  return activeVars;
}


bool
TMVARead::isVariableExist(const string var, const vector<string> trVars, vector<string> &activeVars)
{
  bool check=false;
  
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

bool
TMVARead::StringCompare(const string str1, const string str2)
{

  string a = str1; string b = str2;
  
  transform(a.begin(), a.end(), a.begin(),
		 [](unsigned char c){ return tolower(c); });
  transform(b.begin(), b.end(), b.begin(),
		 [](unsigned char c){ return tolower(c); });
  
  return a.find(b) != string::npos || b.find(a) != string::npos;
}

bool
TMVARead::CheckDigit(const string str)
{
  string negsign = "-";
  bool isDigit=find_if(str.begin(), 
		       str.end(), [&negsign](unsigned char c) { return !isdigit(c) && negsign!=c; }) == str.end();

  if (isDigit){
    double val = atof(str.c_str());
    if (val<=0){
      cout << "Your value " << str << ", has to be positive and different than zero." << endl;
      exit(0);
    }
  }
  
  return isDigit;
}

double
TMVARead:: FindDigit(const string file,const string var){
  auto strPos = file.find(var);
  string str_tmp = file.substr(strPos+var.length());
  double val = atoi(str_tmp.c_str());
  return val;
}
