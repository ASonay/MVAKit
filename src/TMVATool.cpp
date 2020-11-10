#include <thread> 

#include "TSystem.h"

#include "TMVATool/Configuration.hpp"
#include "TMVATool/TMVATool.hpp"

using namespace std;

TMVATool::TMVATool(const char *name) : Configuration(name),
				       m_event_current(0),
				       m_total_param_weight(0)
{}

TMVATool::~TMVATool()
{}

void
TMVATool::EvaluateEvents(const vector<Float_t*> &vec_variables, const vector<Float_t*> &vec_variablesSpec, string name, TString method){
    
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
  }
  if (m_parameterized){
    bdt_name += "_"+m_paramvar+m_param;
    param_val = atof(m_param.c_str());
    cout << "\nThe parameter value : " << param_val << "\n" << endl;
    if (m_param.empty()){
      cout << "You declared ParamVar in the config file, but you did not give any parameter by --par." << endl;
      exit(0);
    }
  }
  
  for (auto ntup : m_ntups){
    string tree_name;
    for (auto tree : m_tree){
      if ( (Common::StringCompare(ntup.first,tree.first)) || tree.first == "All" ) tree_name=tree.second;
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
	vector<Double_t> varsSpec;
	if (m_parameterized) vars.push_back(param_val);
	for (auto var : m_variables_other)
	  {read.SetSingleVariable(var);varsSpec.push_back(read.GetInputSingle(i));}
	for (unsigned rsize=0;rsize<m_readers.size();rsize++){
	  for(unsigned j=0;j<vars.size();j++){
	    vec_variables[rsize][j]=vars[j];
	  }
	  for(unsigned j=0;j<varsSpec.size();j++){
	    vec_variablesSpec[rsize][j]=varsSpec[j];
	  }
	}
	for (unsigned rsize=0;rsize<m_readers.size();rsize++){
	  read.SetSingleVariable(m_cond[rsize]);
	  if (int(read.GetInputSingle(i)))
	    BDT=m_readers[rsize]->EvaluateMVA( method );
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
TMVATool::SetLoaders(const vector<TMVA::DataLoader*> &loaders){
  if (m_loaders.size()==0)
    {m_loaders=loaders;}
  else{
    cout << "You already filled your loaders and you are changing them now!!!" << endl;
  } 
}

void
TMVATool::SetLoaders(TMVA::DataLoader* loader){
  m_loaders.push_back(loader);
}

void
TMVATool::SetReaders(const vector<TMVA::Reader*> &readers){
  if (m_readers.size()==0)
    {m_readers=readers;}
  else{
    cout << "You already filled your readers and you are changing them now!!!" << endl;
  } 
}

void
TMVATool::SetReaders(TMVA::Reader* reader){
  m_readers.push_back(reader);
}

void
TMVATool::SetEvents(){

  const int split_size = m_split.size(); 

  m_dataTrain.resize(split_size);
  m_dataTest.resize(split_size);
  
  if (m_variables.size()<1){
    cout << "Variables has to enter." << endl;
    exit(0);
  }

  if (m_loaders.size()==0){
    cout << "\nNo TMVA loader detected, data will be recorded in containers." << endl;
  }
  else if (m_loaders.size()!=m_split.size()){
    cout << "\nTMVA loaders quantity has to be equal with nfold.." << endl;
    exit(0);
  }
  else{
    cout << "\nTMVA loaders will be filled.." << endl;
  }
  
  cout << "\nVariables:" << endl;
  for (auto var : m_variables)
    {cout << var << endl;}
  
  for (auto x : m_ntups)
    {ReadEvents(x.first,x.second);}

}

void
TMVATool::ReadEvents(string label, vector<string> files){

  m_total=0;
  
  m_label_current = label;
  
  cout << "\nSampling label : " << m_label_current << " for following samples:\n" << endl;

  map<int,double> tmp_param_map;

  const int split_size = m_split.size();   
  
  vector<unsigned> tot_train(split_size,0);
  vector<unsigned> tot_test(split_size,0);
  vector<float> totw_train(split_size,0.0);
  vector<float> totw_test(split_size,0.0);


  if (m_weight.size()==0)
    {m_weight_current="1";}
  else{
    for (auto x : m_weight){
      if (Common::StringCompare(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_weight_current=x.second;}
    }
  }

  if (m_cut.size()==0)
    {m_cut_current="1";}
  else{
    for (auto x : m_cut){
      if (Common::StringCompare(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_cut_current=x.second;}
    }
  }

  if (m_scale.size()==0)
    {m_scale_current["single"]=1.0;}
  else{
    for (auto x : m_scale){
      if (Common::StringCompare(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_scale_current=x.second;}
    }
  }

  string tree_name;
  for (auto x : m_tree){
    if ( (Common::StringCompare(label,x.first)) || (Common::StringCompare("All",x.first)) )
      {tree_name=x.second;}
  }

  cout << "\nScaling will be applied for " << label << ":"<< endl;
  for (auto const& x : m_scale_current){
    cout << "Selection: " << x.first << " Value: " << x.second << endl;
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
    else {
      cout << "Total Number of Events : " << tree->GetEntries(m_cut_current.c_str()) << endl;
    }
    vector<string> activeVars = CheckVars(tree);
    tree->SetBranchStatus("*",0);
    for (auto av : activeVars) tree->SetBranchStatus(av.c_str(),1);
    for (auto val : TreeSplit((int)tree->GetEntries())){
      vector<Counter> counter;
      AssignEvents(tree,x,val,tmp_param_map,counter);
      cout << "Read/Total events : " << setw(8) << val.second << "/"
	   << setw(8) << tree->GetEntries();
      for (int i=0;i<split_size;i++){
	tot_train[i]+=counter[i].count_train;tot_test[i]+=counter[i].count_test;
	totw_train[i]+=counter[i].weight_train;totw_test[i]+=counter[i].weight_test;
	cout << " || Split " << setw(3) << i
	     << " Training/Test events : " << setw(5) << counter[i].count_train
	     << "/" << setw(5) << counter[i].count_test;
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
    cout << "Total count for random generation : " << m_total << endl;
    vector<double> param_vec,weights;
    for (auto const& x : tmp_param_map)
      {
	double param = x.first;
	double ratio = (double)x.second/(double)m_total;
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
      for (auto const& x : tmp_param_map)
	{m_param_map[x.first]=(double)x.second/(double)m_total;}
      m_param_vec=param_vec;
      m_weights=weights;
    }
  }

}

void
TMVATool::AssignEvents(TTree *tr, const string name, pair<Long64_t,Long64_t> split, map<int,double> &tmp_param_map, vector<Counter> &c)
{
  
  const int split_size = m_split.size(); 
  
  vector<unsigned> tmp_train(split_size,0);
  vector<unsigned> tmp_test(split_size,0);
  vector<float> tmpw_train(split_size,0.0);
  vector<float> tmpw_test(split_size,0.0);
    
  random_device rd;
  mt19937 gen(rd());    
  discrete_distribution<int> d(begin(m_weights), end(m_weights));

  TFile *f_tmp = new TFile("f_tmp.root","recreate");
  TTree *tree = tr->CopyTree(m_cut_current.c_str(),"",split.first,split.second);

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
      Double_t scale = 1.0;
      for (auto const& x : m_scale_current){
	if (x.first=="single")
	  {scale=x.second;}
	else if (!Common::CheckDigit(x.first)){
	  read.SetSingleVariable(x.first);
	  if (read.GetInputSingle(i)) {scale=x.second;}
	}
      }
      weight*=scale;
      vector<Double_t> vars = read.GetInput(i);
      vector<Double_t> varsSpec;
      if (m_parameterized) {vars.push_back(GetParam(name,tmp_param_map,weight,gen,d));}
      for (auto var : m_variables_other)
	{read.SetSingleVariable(var);varsSpec.push_back(read.GetInputSingle(i));}
      if (split_cond){
	if (m_loaders.size()!=0){
	  for (auto var : varsSpec) vars.push_back(var);
	  m_loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTraining,vars,weight);
	}
	else{
	  m_dataTrain[cond_index].push_back({vars,varsSpec,weight,m_label_current});
	}
	tmp_train[cond_index]++;
	tmpw_train[cond_index]+=weight;
      }
      else{
	if (m_loaders.size()!=0){
	  for (auto var : varsSpec) vars.push_back(var);
	  m_loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTesting,vars,weight);
	}
	else{
	  m_dataTest[cond_index].push_back({vars,varsSpec,weight,m_label_current});
	}
	tmp_test[cond_index]++;
	tmpw_test[cond_index]+=weight;
      }
      cond_index++;
    }
  }

  for (int i=0;i<split_size;i++) {c.push_back({tmp_test[i],tmp_train[i],tmpw_test[i],tmpw_train[i]});}
  
  f_tmp->Close();
  if( remove("f_tmp.root") != 0 )
    {cout << "\nError deleting temporary root file." << endl;}
}

vector<pair<Long64_t,Long64_t>>
TMVATool::TreeSplit(int noe)
{
  vector<pair<Long64_t,Long64_t>> val;

  int min = 5000;

  if (noe<min){
    return {{noe,0}};
  }
  else{
    for (int i=0;i<((noe-1)/min)+1;i++){
      val.push_back(make_pair(min,min*i));
    }
    return val;
  }
}

double
TMVATool::GetParam(string file,map<int,double> &pmap,Double_t &weight,mt19937 &gen,discrete_distribution<int> &d)
{
  double param=0;

  if (m_param_map.size()==0){
    param = Common::FindDigit(file,m_paramvar);
    Double_t scale = m_scale_current[to_string((int)param)] != 0.0 ? m_scale_current[to_string((int)param)] : 1.0;
    weight*=scale;
    m_param_sample_weights[(int)param]+=weight;
    m_total_param_weight+=weight;
  }
  else{
    int param_index=d(gen);
    param=m_param_vec[param_index];
    Double_t scale = m_scale_current[to_string((int)param)] != 0.0 ? m_scale_current[to_string((int)param)] : 1.0;
    weight*=scale;
    if (Common::StringCompare(m_pscale,"cross") || Common::StringCompare(m_pscale,"section") || Common::StringCompare(m_pscale,"xs")){
      weight*=((Double_t)m_param_sample_weights[(int)param]/(Double_t)m_total_param_weight)
	/((Double_t)m_param_map[(int)param]);
    }
    else if (Common::StringCompare(m_pscale,"flat")){
      weight*=1.0/((Double_t)m_param_map[(int)param]);
    }

  }

  if (Common::StringCompare(m_psplit,"cross") || Common::StringCompare(m_psplit,"section") || Common::StringCompare(m_psplit,"xs"))
    {pmap[(int)param] += (double)weight;m_total += (double)weight;}
  else if (Common::StringCompare(m_psplit,"event") || Common::StringCompare(m_psplit,"number") || Common::StringCompare(m_psplit,"enum"))
    {pmap[(int)param] += 1;m_total += 1.0;}
  else if (Common::StringCompare(m_psplit,"uni") || Common::StringCompare(m_psplit,"uniform"))
    {pmap[(int)param] = 1;m_total = (double)pmap.size();}
  else
    {
      cout << "None of the known parameterization splitting found." << endl;
      cout << "You should better to check," << endl;
      cout << "ParamSplit: " << m_psplit << "in your config file." << endl;
      cout << "Available options: Uni/Uniform, Cross-section/XS, Event number/Enum ..." << endl;
      exit(0);
    }
  
  return param;
}


vector<string>
TMVATool::CheckVars(TTree *tree)
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
    if (!Common::CheckDigit(x.second)){
      if (!isVariableExist(x.second,trVars,activeVars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }

  for (auto x : m_cut){
    if (!Common::CheckDigit(x.second)){
      if (!isVariableExist(x.second,trVars,activeVars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }

  return activeVars;
}


bool
TMVATool::isVariableExist(const string var, const vector<string> trVars, vector<string> &activeVars)
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

int
TMVATool::NextEvent(char *sample,int split_index,int start,int max)
{
  m_split_index=split_index;
  if (m_split_index>(int)m_split.size()){
    cout << "Your split quantity can not be more than splitting samples." << endl;
    exit(0);
  }
  m_event_sample=string(sample);
  int max_size;
  if (Common::StringCompare(m_event_sample,"train")) {
    max_size = (int)m_dataTrain[split_index].size();
  }
  else if (Common::StringCompare(m_event_sample,"test")) {
    max_size = (int)m_dataTest[split_index].size();
  }
  else {
    cout << "Your sample could be testing or training." << endl;
    exit(0);
  }
  if (start!=0) {m_event_current=start;}
  if (max!=-1 && max>m_event_current && max<=max_size) {max_size=max;}

  if (max_size <= m_event_current) {
    cout << "You reached maximum number of events" << endl;
    m_event_current=0;
    return 0;
  }
  else{
    m_event_current++;
    return 1;
  }
}

float
TMVATool::GetWeight()
{
  if (m_event_current==0) {
    cout << "Returning 0. Try your chance with NextEvent." << endl;
    return 0;
  }
  else if (Common::StringCompare(m_event_sample,"train"))
    {return m_dataTrain[m_split_index][m_event_current-1].Weight;}
  else if (Common::StringCompare(m_event_sample,"test"))
    {return m_dataTest[m_split_index][m_event_current-1].Weight;}
  else {
    cout << "Your sample could be testing or training." << endl;
    exit(0);
  }
}

const char *
TMVATool::GetLabel(void)
{
  const char *r_pt;
  if (m_event_current==0) {
    cout << "Returning 0. Try your chance with NextEvent." << endl;
    return r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"train")){
    r_pt=m_dataTrain[m_split_index][m_event_current-1].Label.c_str();
    return r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"test")){
    r_pt=m_dataTest[m_split_index][m_event_current-1].Label.c_str();
    return r_pt;
  }
  else {
    cout << "Your sample could be testing or training." << endl;
    exit(0);
  }
}

float *
TMVATool::GetVars(void)
{
  int n = m_variables.size();
  delete vars_r_pt;
  vars_r_pt = new float[n];
  if (m_event_current==0) {
    cout << "Returning 0. Try your chance with NextEvent." << endl;
    return vars_r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"train")){
    for (int i=0;i<n;i++) {vars_r_pt[i]=m_dataTrain[m_split_index][m_event_current-1].Vars[i];}
    return vars_r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"test")){
    for (int i=0;i<n;i++) {vars_r_pt[i]=m_dataTest[m_split_index][m_event_current-1].Vars[i];}     
    return vars_r_pt;
  }
  else {
    cout << "Your sample could be testing or training." << endl;
    exit(0);
  }
}

float *
TMVATool::GetSpectatorVars(void)
{
  int n = m_variables_other.size();
  delete varsSpec_r_pt;
  varsSpec_r_pt = new float[n];
  if (m_event_current==0) {
    cout << "Returning 0. Try your chance with NextEvent." << endl;
    return varsSpec_r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"train")){
    for (int i=0;i<n;i++) {varsSpec_r_pt[i]=m_dataTrain[m_split_index][m_event_current-1].VarsSpec[i];}
    return varsSpec_r_pt;
  }
  else if (Common::StringCompare(m_event_sample,"test")){
    for (int i=0;i<n;i++) {varsSpec_r_pt[i]=m_dataTest[m_split_index][m_event_current-1].VarsSpec[i];}     
    return varsSpec_r_pt;
  }
  else {
    cout << "Your sample could be testing or training." << endl;
    exit(0);
  }
}
