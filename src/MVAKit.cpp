
#include "TSystem.h"
#include "TBranch.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"

#include "MVAKit/Configuration.hpp"
#include "MVAKit/MVAKit.hpp"
#include <mutex>

using namespace std;
mutex mu;

MVAKit::MVAKit(const char *name) : Configuration(name),
				       is_second(false),
				       m_fsave(false),
				       m_fclone(false),
				       m_fcsv(false),
				       m_maxth(10),
				       m_total_param_weight(0)
{
  m_treader.reset(new ReadTree());
}

MVAKit::~MVAKit()
{}

void
MVAKit::CloseFile()
{
  const int split_size = m_split.size();

  for (int i=0;i<split_size;i++){
    if (m_split_per!=100){
      m_ttree_test[i]->Write();
    }
    m_ttree_train[i]->Write();
  }

  m_tfile->Write();
  
  m_ttree_test.clear();
  m_ttree_train.clear();

  cout << "Closing file.." << endl;

  m_tfile->Close("R");
  
}

void
MVAKit::SetCSV(const char *name)
{
  const string fileName(name);
  m_csvfile.open(fileName);
  m_csvfile << "Class,Train/Test,Split";
  for (auto const &var : m_variables)
    {m_csvfile << "," << var.first;}
  if (m_parameterized)
    {m_csvfile << "," << m_paramvar;}
  for (auto const &var : m_variables_other)
    {m_csvfile << "," << var.first;}
  m_csvfile << "\n";

  m_fcsv = true;
}

void
MVAKit::SetFile(const char *name)
{
  const string base_name_train = "TrainTree";
  const string base_name_test = "TestTree";
  int total_variables = 0, total_vectors = 0;
  for (auto const &var : m_variables){
    if (Common::StringCompare("flat",var.type)) {total_variables++;}
    else if (Common::StringCompare("vector",var.type)) {total_vectors++;}
    else {
      cout << "The known variable types are flat and vector" << endl;
      exit(0);
    }
  }
  for (auto const &var : m_variables_other){
    if (Common::StringCompare("flat",var.type)) {total_variables++;}
    else if (Common::StringCompare("vector",var.type)) {total_vectors++;}
    else {
      cout << "The known variable types are flat and vector" << endl;
      exit(0);
    }
  }
  if (m_parameterized) {total_variables++;}
  const int nov_vars = total_variables;
  const int nov_vecs = total_vectors;
  const int split_size = m_split.size();

  cout << "Number of flat variables " << total_variables << " and vector variables" << total_vectors << " found." << endl;
  
  m_fsave = true;
  m_tfile.reset(new TFile(name,"recreate"));
  m_var_rec.reset(new Double_t[nov_vars]);
  m_vec_rec.reset(new vector<Double_t>[nov_vecs]);

  for (int i=0;i<split_size;i++){
    int counter_flat=0, counter_vec=0;
    m_ttree_test.emplace_back(new TTree((base_name_test+to_string(i)).c_str(),"Test Tree"));
    m_ttree_test.back()->Branch("classID",&m_classID);
    m_ttree_test.back()->Branch("label",&m_label_current);
    for (auto const &var : m_variables){
      if (Common::StringCompare("flat",var.type)) {
	m_ttree_test.back()->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
	counter_flat++;
      }
      else if (Common::StringCompare("vector",var.type)) {
	m_ttree_test.back()->Branch(var.second.c_str(),&m_vec_rec[counter_vec]);
	counter_vec++;
      }
      else {
	cout << "The known variable types are flat and vector" << endl;
	exit(0);
      }
    }
    if (m_parameterized){
      m_ttree_test.back()->Branch(m_paramvar.c_str(),&m_var_rec[counter_flat],(m_paramvar+"/D").c_str());
      counter_flat++;
    }
    for (auto const &var : m_variables_other){
      if (Common::StringCompare("flat",var.type)) {
	m_ttree_test.back()->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
	counter_flat++;
      }
      else if (Common::StringCompare("vector",var.type)) {
	m_ttree_test.back()->Branch(var.second.c_str(),&m_vec_rec[counter_vec]);
	counter_vec++;
      }
      else {
	cout << "The known variable types are flat and vector" << endl;
	exit(0);
      }
    }
    m_ttree_test.back()->Branch("weight",&m_w,"weight/D");
  }
  for (int i=0;i<split_size;i++){
    int counter_flat=0, counter_vec=0;
    m_ttree_train.emplace_back(new TTree((base_name_train+to_string(i)).c_str(),"Train Tree"));
    m_ttree_train.back()->Branch("classID",&m_classID);
    m_ttree_train.back()->Branch("label",&m_label_current);
    for (auto const &var : m_variables){
      if (Common::StringCompare("flat",var.type)) {
	m_ttree_train.back()->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
	counter_flat++;
      }
      else if (Common::StringCompare("vector",var.type)) {
	m_ttree_train.back()->Branch(var.second.c_str(),&m_vec_rec[counter_vec]);
	counter_vec++;
      }
      else {
	cout << "The known variable types are flat and vector" << endl;
	exit(0);
      }
    }
    if (m_parameterized){
      m_ttree_train.back()->Branch(m_paramvar.c_str(),&m_var_rec[counter_flat],(m_paramvar+"/D").c_str());
      counter_flat++;
    }
    for (auto const &var : m_variables_other){
      if (Common::StringCompare("flat",var.type)) {
	m_ttree_train.back()->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
	counter_flat++;
      }
      else if (Common::StringCompare("vector",var.type)) {
	m_ttree_train.back()->Branch(var.second.c_str(),&m_vec_rec[counter_vec]);
	counter_vec++;
      }
      else {
	cout << "The known variable types are flat and vector" << endl;
	exit(0);
      }
    }
    m_ttree_train.back()->Branch("weight",&m_w,"weight/D");
  }
}

void
MVAKit::SetFiletoClone(string name)
{
  string newname(name);
  string substr = ".root";
  newname.replace(newname.find(substr),substr.length(),"_clone.root");

  int total_variables = m_variables.size()+m_variables_other.size();
  if (m_parameterized) {total_variables++;}
  const int nov = total_variables;
  m_var_rec.reset(new Double_t[nov]);

  m_tchain.reset(new TChain(m_tree_current.c_str()));
  m_tchain->Add(name.c_str());
  m_tfile.reset(new TFile(newname.c_str(),"recreate"));
  m_ttree = unique_ptr<TTree>(m_tchain->CloneTree());

  m_ttree->Branch("classID",&m_classID);
  m_ttree->Branch("label",&m_label_current);
  int counter_flat=0, counter_vec=0;
  for (auto const &var : m_variables){
    if (Common::StringCompare("flat",var.type)) {
      m_ttree->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
      counter_flat++;
    }
    else if (Common::StringCompare("vector",var.type)) {
      m_ttree->Branch(var.second.c_str(),&m_var_rec[counter_vec]);
      counter_vec++;
    }
    else {
      cout << "The known variable types are flat and vector" << endl;
      exit(0);
    }
  }
  if (m_parameterized){
    m_ttree->Branch(m_paramvar.c_str(),&m_var_rec[counter_flat],(m_paramvar+"/D").c_str());
    counter_flat++;
  }
  for (auto const &var : m_variables_other){
    if (Common::StringCompare("flat",var.type)) {
      m_ttree->Branch(var.second.c_str(),&m_var_rec[counter_flat],(var.second+"/D").c_str());
      counter_flat++;
    }
    else if (Common::StringCompare("vector",var.type)) {
      m_ttree->Branch(var.second.c_str(),&m_var_rec[counter_vec]);
      counter_vec++;
    }
    else {
      cout << "The known variable types are flat and vector" << endl;
      exit(0);
    }
  }
  m_ttree->Branch("weight",&m_w,"weight/D");
}

void
MVAKit::WritetoCSV(string type, int split)
{
  m_csvfile << m_label_current << "," << type << "," << split;
  for (auto var : m_vars) {m_csvfile << "," << var;}
  for (auto var : m_varsSpec) {m_csvfile << "," << var;}
  m_csvfile << "\n";
}

bool
MVAKit::FillVarstoRecord()
{

  if (m_fsave){
    int counter_flat=0,counter_vec=0;
    for (auto x : m_vars){
      m_var_rec[counter_flat]=x;
      counter_flat++;
    }
    for (auto x : m_varsSpec){
      m_var_rec[counter_flat]=x;
      counter_flat++;
    }
    for (auto x : m_vecs){
      m_vec_rec[counter_vec]=x;
      counter_vec++;
    }
    for (auto x : m_vecsSpec){
      m_vec_rec[counter_vec]=x;
      counter_vec++;
    }
  }
  
  if (m_fclone) {
    m_ttree->GetBranch("classID")->Fill();
    m_ttree->GetBranch("label")->Fill();
    for (auto const &var : m_variables) {m_ttree->GetBranch(var.second.c_str())->Fill();}
    for (auto const &var : m_variables_other) {m_ttree->GetBranch(var.second.c_str())->Fill();}
    m_ttree->GetBranch("weight")->Fill();
    return true;
  }
  else {return false;}
}

void
MVAKit::SetLoaders(const vector<TMVA::DataLoader*> &loaders){
  if (m_loaders.size()==0)
    {m_loaders=loaders;}
  else{
    cout << "You already filled your loaders and you are changing them now!!!" << endl;
  } 
}

void
MVAKit::SetLoaders(TMVA::DataLoader* loader){
  m_loaders.push_back(loader);
}

void
MVAKit::SetEvents(const char *file,const char *label,int doCut){

  const string fileName(file);
  const string labelName(label);
  if (!doCut) m_cut.clear();
  
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
  else if (m_loaders.size()!=(unsigned)split_size){
    cout << "\nTMVA loaders quantity has to be equal with nfold.." << endl;
    exit(0);
  }
  else{
    cout << "\nTMVA loaders will be filled.." << endl;
  }

  cout << "\nVariables:" << endl;
  for (auto const &var : m_variables)
    {cout << "Name: " << var.second << " Var(s): " << var.first << endl;}
  
  m_treader->ResetVariables();
  m_treader->SetAlias(m_alias);
  for (auto const &var : m_variables) {
    string title = "v";
    if (Common::StringCompare("vector",var.type)) {title="vec";}
    m_treader->SetVariables(title,var.first);
  }
  for (auto const &var : m_variables_other) {
    string title = "vs";
    if (Common::StringCompare("vector",var.type)) {title="vecs";}
    m_treader->SetVariables(title,var.first);
  }
  if (m_split_per==0)
    m_treader->SetVariables("z",m_split);
  m_treader->ProcessVariables();

  if (fileName.empty()){
    for (auto x : m_ntups)
      {ReadEvents(x.first,x.second);}
  }else {ReadEvents(labelName,{fileName});}
}

void
MVAKit::ReadEvents(string label, vector<string> files){

  m_total=0;
  m_weight_current.clear();
  m_cut_current.clear();
  m_scale_current.clear();

  m_label_current = label;

 
  cout << "\nSampling label : " << m_label_current << " for following samples:\n" << endl;

  if (m_weight.size()==0)
    {m_weight_current="1";}
  else{
    for (auto x : m_weight){
      if (Common::StringFind(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_weight_current=x.second;}
    }
    if (m_weight_current.empty()){
      cout << "Your weight couldn't match with any label in " << label <<" , will be use 1 instead." << endl;
      m_weight_current="1";
    }
  }

  if (m_cut.size()==0)
    {m_cut_current="1";}
  else{
    for (auto x : m_cut){
      if (Common::StringFind(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_cut_current=x.second;}
    }
    if (m_cut_current.empty()){
      cout << "Your cut couldn't match with any label in " << label <<" , will be use 1 instead." << endl;
      m_cut_current="1";
    }
  }

  if (m_scale.size()==0)
    {m_scale_current["single"]=1.0;}
  else{
    for (auto x : m_scale){
      if (Common::StringFind(label,x.first) || (Common::StringCompare("All",x.first)))
	{m_scale_current=x.second;}
    }
    if (m_scale_current.empty()){
      cout << "Your scale couldn't match with any label in " << label <<" , will be use 1 instead." << endl;
      m_scale_current["single"]=1.0;
    }
  }

  for (auto x : m_tree){
    if ( (Common::StringFind(label,x.first)) || (Common::StringCompare("All",x.first)) )
      {m_tree_current=x.second;}
  }

  cout << "\nScaling will be applied for " << label << ":"<< endl;
  for (auto const& x : m_scale_current){
    cout << "Selection: " << x.first << " Value: " << x.second << endl;
  }

  if (m_tree_current.empty()){
    cout << "Cannot find any tree name declared in the configuration file or non of them match with the label" << endl;
    exit(0);
  }

  m_classID = ConvertLabelOpt();
  
  cout << "CUT EXPRESSION: " << m_cut_current << endl;
  cout << "WEIGHT EXPRESSION: " << m_weight_current << "\n" << endl;

  if (m_parameterized && !is_second && Common::StringCompare(m_psplit,"uni")){
    double total_weight = 0;
    for (auto x : files) {
      cout << "File will be open for unificated mass distribution: " << x << endl;
      m_treader->SetFormulas(x,m_tree_current,m_cut_current,m_weight_current);
      double param = 0;
      if (m_paramFile.empty()) {
	param = Common::FindDigit(x,m_paramvar);
      }
      else {
	param = Common::FindDigit(x,Common::StringSep(m_paramFile,','));
      }
      double weight = 0;
      while (m_treader->NextEvent()){
	weight += m_treader->GetWeight();
      }
      total_weight += weight;
      m_scale_current[to_string((int)param)] += weight;
      cout << "Weight is " << weight << " for " << param << endl;
    }
    cout << "Total weight : " << total_weight << endl;
    for (auto &x : m_scale_current) {
      cout << "Weight calculated " << x.second << " for " << x.first << endl;      
      x.second = total_weight / x.second;
      cout << "will be scaled by " << x.second << endl;
    }
  }
  
  for (auto x : files){
    AssignEvents(x);
  }

  cout << "\nTotal count and weight for label, " << m_label_current << ": "
       << m_LabelEntries[m_label_current] << ", " << m_LabelWeight[m_label_current]
       << "\n"<< endl; 
  
  if (m_parameterized && !is_second){
    cout << "\nParameterization:" << endl;
    cout << "Total count for random generation : " << m_total << endl;
    vector<double> param_vec,weights;
    for (auto const& x : m_param_map)
      {
	double param = x.first;
	double ratio = (double)x.second/(double)m_total;
	cout << param
	     << ':' 
	     << x.second
	     << ':'
	     << ratio
	     << endl ;
	m_param_vec.push_back(param);
	m_weights.push_back(ratio);
      }
    is_second = true;
    m_gen = mt19937(1701);
    m_discDist = discrete_distribution(begin(m_weights), end(m_weights));
  }

}

void
MVAKit::AssignEvents(const string fname)
{

  cout << "File will be open: " << fname << endl;
  m_treader->SetFormulas(fname,m_tree_current,m_cut_current,m_weight_current);
  if (m_fclone) {SetFiletoClone(fname);}

  while (m_treader->NextEvent()){
    m_w = m_treader->GetWeight();
    Double_t scale = 1.0;
    for (auto const& x : m_scale_current){
      if (x.first=="single")
	{scale=x.second;}
    }
    m_w*=scale; 
    m_LabelWeight[m_label_current]+=m_w;
    m_LabelEntries[m_label_current]+=1;
    m_vars = m_treader->GetInputs("v");
    m_varsSpec = m_treader->GetInputs("vs");
    m_vecs = m_treader->GetVectors("vec");
    m_vecsSpec = m_treader->GetVectors("vecs");
    if (m_parameterized) {m_vars.push_back((Double_t)GetParam(fname,m_w));}
    vector<Double_t> cond = m_treader->GetInputs("z");
    int cond_index=0;
    for (auto sp : m_split){
      int split_cond=0;
      if (m_split_per==0) {
	split_cond=int(cond[cond_index]);
      }
      else {split_cond = int(rand()%100 < m_split_per);}
      if (FillVarstoRecord()) {continue;}
      if (split_cond){
	if (m_loaders.size()!=0){
	  for (auto var : m_varsSpec) m_vars.push_back(var);
	  m_loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTraining,m_vars,m_w);
	}else if (m_fsave){m_ttree_train[cond_index]->Fill();}
	else if (m_fcsv){WritetoCSV("train",cond_index);}
	else {cout << "Not find any SetFile or SetLoader" << endl;exit(0);}
      }
      else{
	if (m_loaders.size()!=0){
	  for (auto var : m_varsSpec) m_vars.push_back(var);
	  m_loaders[cond_index]->AddEvent(m_label_current,TMVA::Types::kTesting,m_vars,m_w);
	}else if (m_fsave){m_ttree_test[cond_index]->Fill();}
	else if (m_fcsv){WritetoCSV("test",cond_index);}
	else {cout << "Not find any SetFile or SetLoader" << endl;exit(0);}
      }
      cond_index++;
    }
  }
  if (m_fclone) {
    m_ttree->Write();
    m_tfile->Write();
    m_ttree.reset();
  }
}

double
MVAKit::GetParam(string file,Double_t &weight)
{
  double param=0;

  if (!is_second){
    if (m_paramFile.empty()) {
      param = Common::FindDigit(file,m_paramvar);
    }
    else {
      param = Common::FindDigit(file,Common::StringSep(m_paramFile,','));
    }
    Double_t scale = m_scale_current[to_string((int)param)] != 0.0 ? m_scale_current[to_string((int)param)] : 1.0;
    weight*=scale;
    m_param_sample_weights[(int)param]+=weight;
    m_total_param_weight+=weight;
    
    if (Common::StringCompare(m_psplit,"cross") || Common::StringCompare(m_psplit,"section") || Common::StringCompare(m_psplit,"xs"))
      {m_param_map[(int)param] += (double)weight;m_total += (double)weight;}
    else if (Common::StringCompare(m_psplit,"event") || Common::StringCompare(m_psplit,"number") || Common::StringCompare(m_psplit,"enum"))
      {m_param_map[(int)param] += 1;m_total += 1.0;}
    else if (Common::StringCompare(m_psplit,"uni") || Common::StringCompare(m_psplit,"uniform"))
      {m_param_map[(int)param] = 1;m_total = (double)m_param_map.size();}
    else
      {
	cout << "None of the known parameterization splitting found." << endl;
	cout << "You should better to check," << endl;
	cout << "ParamSplit: " << m_psplit << "in your config file." << endl;
	cout << "Available options: Uni/Uniform, Cross-section/XS, Event number/Enum ..." << endl;
	exit(0);
      }
  
  }
  else{
    int param_index=m_discDist(m_gen);
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

  return param;
}

void
MVAKit::ExecuteModel()
{
  TString express = "create_model.py "+to_string(m_nvar)+" \""+m_arcOpt+"\" \""+m_engOpt+"\"";
  UInt_t ret = gSystem->Exec(express);
  if(ret!=0){
    cout << "[ERROR] Failed to generate model using python" << endl;
    exit(0);
  }
}

vector<int>
MVAKit::ConvertLabelOpt()
{
  if (!string(m_labOpt).empty()){
    vector<int> vec;
    vector<string> labs = Common::StringSep(string(m_labOpt),',');
    for (auto lab : labs){
      vector<string> l = Common::StringSep(lab,';');
      if (Common::StringFind(l[0],m_label_current)){
	cout << "Label Opt : " << l[1] << endl;
	string str = "ClassID";
	for (auto x : Common::StringSep(l[1],'|')){
	  vec.push_back(atoi(x.c_str()));
	  str += x;
	}
	m_label_current=str;
	return vec;
      }
    }
  }
  
  return {0};
}
