#include "TSystem.h"

#include "../inc/TMVAConf.hpp"


using namespace std;

TMVAConf::TMVAConf(const string name):
  m_split_per(0),
  m_parameterized(false)
{
  cout << "Configuration has been registered for " << name << ".\n" << endl;
  m_trainingOpt = "!H:!V:NTrees=600:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.6:SeparationType=GiniIndex:nCuts=30:MaxDepth=2:NegWeightTreatment=IgnoreNegWeightsInTraining";
  m_factoryOpt = "!V:!Silent:Transformations=I;D;P;G,D:AnalysisType=Classification";
  m_samplingOpt = "nTrain_Signal=0:nTrain_Background=0:NormMode=EqualNumEvents:V";
  m_classifierOpt = "BDT";
}

TMVAConf::~TMVAConf()
{}

void
TMVAConf::Parser(int argc,char **argv)
{
  bool is_ntup=false;
  bool is_file=false;

  for (int i=1;i<argc;i++){
    string check = argv[i];
    //cout << check << endl;
    if (check=="--ntup")
      {
	vector<string> tmp;
	for (int j=i+1;j<argc;j++){
	  check = argv[j];
	  if (check.find("root") != string::npos)
	    {tmp.push_back(check);is_ntup=true;}
	  else
	    {
	      m_ntups.push_back(make_pair(check,tmp));
	      i=j;
	      break;
	    }
	}
      }
    else if ((check=="--ConfFile" || check=="--conf") && !is_file)
      {m_conFile = argv[i+1];is_file=true;i++;}
    else{
      cout << "Usage of the script :" << endl;
      cout << "./tmva_trainer --ntup <ntup1.root> label1 --ntup <ntup2.root> label2 --ConfFile <ConfFile.txt>" << endl;
      exit(0);
    }
  }

  if (!is_ntup){
    cout << "Usage of the script :" << endl;
    cout << "./tmva_trainer --ntup <ntup1.root> label1 --ntup <ntup2.root> label2 --ConfFile <ConfFile.txt>" << endl;
    exit(0);
  }

  if (!is_file){
    cout << "Usage of the script :" << endl;
    cout << "./tmva_trainer --ntup <ntup1.root> label1 --ntup <ntup2.root> label2 --ConfFile <ConfFile.txt>" << endl;
    exit(0);
  }

  cout << "\nConfiguration File: " << m_conFile << endl;
}

void
TMVAConf::ReadConf(){
  ifstream in(m_conFile);
  string str;

  if (in.fail()) {cout << "Your variable list is missing!\n"; exit(0);}
  
  while (in >> str){
    if (str.compare("Var:")==0){
      in >> str;
      m_variables.push_back(str);
    }
    else if (str.compare("OtherVar:")==0){
      in >> str;
      m_variables_other.push_back(str);
    }
    else if (str.compare("ParamVar:")==0){
      in >> str;
      m_paramvar=str;
    }
    else if (str.find("Weight:")!=string::npos){
      auto strPos = str.find("Weight:");
      string label = str.substr(0,strPos);
      in >> str;
      m_weight.push_back(make_pair(label,str));
    }
    else if (str.find("Cut:")!=string::npos){
      auto strPos = str.find("Cut:");
      string label = str.substr(0,strPos);
      in >> str;
      m_cut.push_back(make_pair(label,str));
    }
    else if (str.compare("TrainingOpt:")==0){
      in >> str;
      m_trainingOpt=str;
    }
    else if (str.compare("FactoryOpt:")==0){
      in >> str;
      m_factoryOpt=str;
    }
    else if (str.compare("ClassifierOpt:")==0){
      in >> str;
      m_classifierOpt=str;
    }
    else if (str.compare("SamplingOpt:")==0){
      in >> str;
      m_samplingOpt=str;
    }
    else if (str.compare("Split:")==0){
      in >> str;
      m_split=comaSep(str);
    }
    else if (str.compare("LoadFile:")==0){
      in >> str;
      m_loadFile=str;
    }
    else{
      cout << "Your config file is badly formatted.." << endl;
      cout << "Check here: " << str << endl;
      exit(0);
    }
  }
  if (m_split.size()==0)
    m_split.push_back("Split:50:50");

  if (m_split[0].find("Split:") != string::npos)
    {m_split_per = atoi(m_split[0].substr(6).c_str());}

  if (!m_paramvar.empty()){
    cout << "\nParameterization detected for : " << m_paramvar << endl;
    m_parameterized=true;
  }

  if (!m_loadFile.empty()){
    if (!gSystem->Load(m_loadFile.c_str())){
      cout << "Can not load the file : " << m_loadFile << endl;
      exit(0);
    }
    else cout << m_loadFile << " loaded successfully..\n" << endl;
  }

  if (m_parameterized){
    const int columnId = 0;
    const string paramFind = m_paramvar;
    sort(m_ntups.begin(), m_ntups.end(),
	 [&columnId,&paramFind](const pair<string,vector<string>> &a, const pair<string,vector<string>> &b) 
	 {
	   return a.second[columnId].find(paramFind) != string::npos && b.second[columnId].find(paramFind) == string::npos;
	 });
    if (m_ntups[0].second[0].find(m_paramvar) == string::npos){
      cout << "Your parameter : " << m_paramvar << "does not match with any file." << endl;
      exit(0);
    }
  }
}

void
TMVAConf::SetEvents(const vector<TMVA::DataLoader*> &loaders){
  
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
TMVAConf::ReadEvents(const vector<TMVA::DataLoader*> &loaders, string label, vector<string> files){
  
  cout << "\nSampling label : " << label << " for following samples:\n" << endl;

  map<int,int> tmp_param_map;
  
  unsigned tot_train=0,tot_test=0;
  float totw_train=0,totw_test=0;

  string cut_ex,weight_ex;

  if (m_weight.size()==0)
    {weight_ex="1";}
  else{
    for (auto x : m_weight){
      if (StringCompare(label,x.first)) weight_ex=x.second;
    }
  }

  if (m_cut.size()==0)
    {cut_ex="1";}
  else{
    for (auto x : m_cut){
      if (StringCompare(label,x.first)) cut_ex=x.second;
    }
  }

  if (weight_ex.empty()){
    cout << "You defined weight but not matched any of the label!" << endl;
    exit(0);
  }

  if (cut_ex.empty()){
    cout << "You defined cut but not matched any of the label!" << endl;
    exit(0);
  }

  cout << "CUT EXPRESSION: " << cut_ex << endl;
  cout << "WEIGHT EXPRESSION: " << weight_ex << "\n" << endl;
  
  random_device rd;
  mt19937 gen(rd());    
  discrete_distribution<int> d(begin(m_weights), end(m_weights));

  for (auto x: files){
    cout << "\nFile name to be readed : " << x << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      exit(0);
    }
    TTree *tree = (TTree*) f->Get("nominal_Loose");
    CheckVars(tree);
    ReadTree read(x,tree,m_variables);
    unsigned tmp_train=0,tmp_test=0;
    int noe = read.GetNoE();
    for (int i=0;i<noe;i++){
      read.SetSingleVariable(cut_ex);
      if (int(read.GetInputSingle(i))==0) continue;
      unsigned cond_index=0;
      for (auto sp : m_split){
	int split_cond=0;
	if (m_split_per==0) {
	  read.SetSingleVariable(sp);split_cond=int(read.GetInputSingle(i));
	}
	else {split_cond = int(rand()%100 < m_split_per);}
	read.SetSingleVariable(weight_ex);
	Double_t weight = read.GetInputSingle(i);
	vector<Double_t> vars = read.GetInput(i);
	if (m_parameterized) vars.push_back(GetParam(x,tmp_param_map,gen,d));
	for (auto var : m_variables_other) {read.SetSingleVariable(var);vars.push_back(read.GetInputSingle(i));}
	if (split_cond){
	  loaders[cond_index]->AddEvent(label,TMVA::Types::kTraining,vars,weight);
	  tmp_train++;
	  totw_train+=weight;
	}
	else{
	  loaders[cond_index]->AddEvent(label,TMVA::Types::kTesting,vars,weight);
	  tmp_test++;
	  totw_test+=weight;
	}
	cond_index++;
      }
    }
    tot_train+=tmp_train;tot_test+=tmp_test;
    cout << "Training events : " << tmp_train << endl;
    cout << "Test events : " << tmp_test << endl;
    f->Close();
    cout << "\n" <<endl;
  }

  cout << label << " SAMPLE INFO:" << endl;
  cout << "Total training events: " << tot_train << ", weighted: " << totw_train << endl;
  cout << "Total test events:     " << tot_test << ", weighted: " << totw_test << endl;

  if (m_parameterized){
    cout << "\nParameterization:" << endl;
    unsigned total_event = tot_train+tot_test;
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

vector<string>
TMVAConf::comaSep(const string str){
  stringstream ss( str );
  vector<string> result;

  while( ss.good() )
    {
      string substr;
      getline( ss, substr, ',' );
      result.push_back( substr );
    }

  return result;
}

double
TMVAConf:: FindDigit(const string file,const string var){
  auto strPos = file.find(var);
  string str_tmp = file.substr(strPos+var.length());
  double val = atoi(str_tmp.c_str());
  return val;
}

vector<TString>
TMVAConf::GetTVariables(){
  vector<TString> newvars;
  for (auto var : m_variables){
    string var_tmp = var;
    replace( var_tmp.begin(), var_tmp.end(), '<', 's');
    replace( var_tmp.begin(), var_tmp.end(), '>', 'g');
    replace( var_tmp.begin(), var_tmp.end(), '*', '_');
    replace( var_tmp.begin(), var_tmp.end(), '=', 'e');
    replace( var_tmp.begin(), var_tmp.end(), '+', '_');
    replace( var_tmp.begin(), var_tmp.end(), '-', '_');
    replace( var_tmp.begin(), var_tmp.end(), '.', '_');
    replace( var_tmp.begin(), var_tmp.end(), '&', '_');
    replace( var_tmp.begin(), var_tmp.end(), '*', '_');
    replace( var_tmp.begin(), var_tmp.end(), '{', '_');
    replace( var_tmp.begin(), var_tmp.end(), '}', '_');
    replace( var_tmp.begin(), var_tmp.end(), '(', '_');
    replace( var_tmp.begin(), var_tmp.end(), ')', '_');
    replace( var_tmp.begin(), var_tmp.end(), '[', '_');
    replace( var_tmp.begin(), var_tmp.end(), ']', '_');
    replace( var_tmp.begin(), var_tmp.end(), '$', '_');
    newvars.push_back(var_tmp);
  }

  if (m_parameterized){
    newvars.push_back(m_paramvar);
  }
  
  return newvars;
}

vector<TString>
TMVAConf::GetTVariablesOther(){
  vector<TString> newvars;
  for (auto var : m_variables_other){
    string var_tmp = var;
    replace( var_tmp.begin(), var_tmp.end(), '<', 's');
    replace( var_tmp.begin(), var_tmp.end(), '>', 'g');
    replace( var_tmp.begin(), var_tmp.end(), '*', '_');
    replace( var_tmp.begin(), var_tmp.end(), '=', 'e');
    replace( var_tmp.begin(), var_tmp.end(), '+', '_');
    replace( var_tmp.begin(), var_tmp.end(), '-', '_');
    replace( var_tmp.begin(), var_tmp.end(), '.', '_');
    replace( var_tmp.begin(), var_tmp.end(), '&', '_');
    replace( var_tmp.begin(), var_tmp.end(), '*', '_');
    replace( var_tmp.begin(), var_tmp.end(), '{', '_');
    replace( var_tmp.begin(), var_tmp.end(), '}', '_');
    replace( var_tmp.begin(), var_tmp.end(), '(', '_');
    replace( var_tmp.begin(), var_tmp.end(), ')', '_');
    replace( var_tmp.begin(), var_tmp.end(), '[', '_');
    replace( var_tmp.begin(), var_tmp.end(), ']', '_');
    replace( var_tmp.begin(), var_tmp.end(), '$', '_');
    newvars.push_back(var_tmp);
  }
  return newvars;
}

double
TMVAConf::GetParam(string file,map<int,int> &pmap,mt19937 &gen,discrete_distribution<int> &d)
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

void
TMVAConf::CheckVars(TTree *tree)
{
  vector<string> trvars;

  for (auto x : *tree->GetListOfLeaves()) trvars.push_back(x->GetName());

  for (auto x : m_variables){
    if (!isVariableExist(x,trvars)){
      cout << x << " is not include any of the leaf in the tree." << endl;
      exit(0);
    }
  }
  
  for (auto x : m_variables_other){
    if (!isVariableExist(x,trvars)){
      cout << x << " is not include any of the leaf in the tree." << endl;
      exit(0);
    }
  }

  if (m_split_per==0){
    for (auto x : m_split){
      if (!isVariableExist(x,trvars)){
	cout << x << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }
  
  for (auto x : m_weight){
    if (!CheckDigit(x.second)){
      if (!isVariableExist(x.second,trvars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }

  for (auto x : m_cut){
    if (!CheckDigit(x.second)){
      if (!isVariableExist(x.second,trvars)){
	cout << x.second << " is not include any of the leaf in the tree." << endl;
	exit(0);
      }
    }
  }
}

bool
TMVAConf::CheckDigit(const string str)
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

bool
TMVAConf::isVariableExist(const string var, const vector<string> trvars)
{
  bool check=false;
  
  for (auto y : trvars){
    if (var.find(y) != string::npos){
      check=true;
    }
  }

  return check;  
}

bool
TMVAConf::StringCompare(const string str1, const string str2)
{

  string a = str1; string b = str2;
  
  transform(a.begin(), a.end(), a.begin(),
		 [](unsigned char c){ return tolower(c); });
  transform(b.begin(), b.end(), b.begin(),
		 [](unsigned char c){ return tolower(c); });
  
  return a.find(b) != string::npos || b.find(a) != string::npos;
}
