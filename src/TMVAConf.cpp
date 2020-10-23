#include "../inc/TMVAConf.hpp"


using namespace std;

TMVAConf::TMVAConf(const string name):
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
  int index_sig=0;
  int index_bkg=0;
  int index_conf=0;
  int tot_file_sig=0;
  int tot_file_bkg=0;
  bool is_ntupSig=false;
  bool is_ntupBkg=false;
  bool is_file=false;
  
  for (int i=1;i<argc;i++){
    string check = argv[i];
    //cout << check << endl;
    if (check=="--ntupSig")
      {index_sig=i;is_ntupSig=true;is_ntupBkg=false;}
    else if (check=="--ntupBkg")
      {index_bkg=i;is_ntupSig=false;is_ntupBkg=true;}
    else if (check=="--ConfFile")
      {index_conf=i;is_ntupSig=false;is_ntupBkg=false;is_file=true;i++;}
    else if (check.find("root") != string::npos)
      {
	if (is_ntupSig) tot_file_sig++;
	if (is_ntupBkg) tot_file_bkg++;
      }
    else{
      cout << "Usage of the script :" << endl;
      cout << "./tmva_trainer --ntupSig <ntupSig.root> --ntupBkg <ntupBkg.root> --ConfFile <ConfFile.txt>" << endl;
      exit(0);
    }
  }

  if (index_sig==0||index_bkg==0){
    cout << "Usage of the script :" << endl;
    cout << "./tmva_trainer --ntupSig <ntupSig.root> --ntupBkg <ntupBkg.root> --ConfFile <ConfFile.txt>" << endl;
    exit(0);
  }

  if (!is_file){
    cout << "Usage of the script :" << endl;
    cout << "./tmva_trainer --ntupSig <ntupSig.root> --ntupBkg <ntupBkg.root> --ConfFile <ConfFile.txt>" << endl;
    exit(0);
  }

  for (int i=index_sig+1;i<index_sig+tot_file_sig+1;i++){
    m_ntupsSig.push_back(argv[i]);
  }

  for (int i=index_bkg+1;i<index_bkg+tot_file_bkg+1;i++){
    m_ntupsBkg.push_back(argv[i]);
  }

  m_conFile = argv[index_conf+1];

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
    else if (str.compare("SigWeight:")==0){
      in >> str;
      m_wsig=str;
    }
    else if (str.compare("BkgWeight:")==0){
      in >> str;
      m_wbkg=str;
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
    else if (str.compare("SigCut:")==0){
      in >> str;
      m_csig=str;
    }
    else if (str.compare("BkgCut:")==0){
      in >> str;
      m_cbkg=str;
    }
    else if (str.compare("Split:")==0){
      in >> str;
      m_split=comaSep(str);
    }
    else{
      cout << "Your config file is badly formatted.." << endl;
      exit(0);
    }
  }

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

  if (!m_paramvar.empty()){
    cout << "\nParameterization detected for : " << m_paramvar << endl;
    m_parameterized=true;
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

void
TMVAConf::SetEvents(const vector<TMVA::DataLoader*> &loaders){

  map<int,int> param_map_sig;
  map<int,int> param_map_bkg;
  
  unsigned tot_sig_train=0,tot_sig_test=0;
  unsigned tot_bkg_train=0,tot_bkg_test=0;
  float totw_sig_train=0,totw_sig_test=0;
  float totw_bkg_train=0,totw_bkg_test=0;

  if (m_variables.size()<1){
    cout << "Variables has to enter." << endl;
    exit(0);
  }
  
  cout << "\nVariables:" << endl;
  for (auto var : m_variables)
    cout << var << endl;

  for (auto x: m_ntupsSig){
    cout << "\nFile name to be readed : " << x << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      return;
    }
    TTree *tree = (TTree*) f->Get("nominal_Loose");
    ReadTree read(x,tree,m_variables);
    double param = 0;
    if (m_parameterized){
      param = FindDigit(x,m_paramvar);
      cout << "The Parameter found : "<< param << endl;
    }
    unsigned tmp_sig_train=0,tmp_sig_test=0;
    for (int i=0;i<read.GetNoE();i++){
      read.SetSingleVariable(m_csig);
      if (int(read.GetInputSingle(i))==0) continue;
      unsigned cond_index=0;
      for (auto sp : m_split){
	read.SetSingleVariable(sp);
	int split_cond = int(read.GetInputSingle(i));
	read.SetSingleVariable(m_wsig);
	Double_t weight = read.GetInputSingle(i);
	vector<Double_t> vars = read.GetInput(i);
	if (m_parameterized) vars.push_back(param);
	if (split_cond){
	  loaders[cond_index]->AddSignalTrainingEvent(vars,weight);
	  tmp_sig_train++;
	  totw_sig_train+=weight;
	}
	else{
	  loaders[cond_index]->AddSignalTestEvent(vars,weight);
	  tmp_sig_test++;
	  totw_sig_test+=weight;
	}
	cond_index++;
      }
    }
    tot_sig_train+=tmp_sig_train;
    tot_sig_test+=tmp_sig_test;
    param_map_sig[param]+=tmp_sig_train+tmp_sig_test;
    cout << "Training events : " << tmp_sig_train << endl;
    cout << "Test events : " << tmp_sig_test << endl;
    f->Close();
    cout << "\n" <<endl;
  }

  cout << "SIGNAL SAMPLE INFO:" << endl;
  cout << "Total training events: " << tot_sig_train << ", weighted: " << totw_sig_train << endl;
  cout << "Total test events:     " << tot_sig_test << ", weighted: " << totw_sig_test << endl;

  vector<double> weights;
  vector<double> param_vec;
  if (m_parameterized){
    cout << "\nParameterization:" << endl;
    unsigned total_sig = tot_sig_train+tot_sig_test;
    for (auto const& x : param_map_sig)
      {
	double param = x.first;
	double ratio = 1.0*((double)x.second/(double)total_sig);
	cout << param
	     << ':' 
	     << x.second
	     << ':'
	     << ratio
	     << endl ;
	param_vec.push_back(param);
	weights.push_back(ratio);
      }
  }
  
  random_device rd;
  mt19937 gen(rd());
  discrete_distribution<int> d(begin(weights), end(weights));

  for (auto x: m_ntupsBkg){
    cout << "\nFile name to be readed : " << x << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      return;
    }
    TTree *tree = (TTree*) f->Get("nominal_Loose");
    ReadTree read(x,tree,m_variables);
    unsigned tmp_bkg_train=0,tmp_bkg_test=0;
    double param = 0;
    for (int i=0;i<read.GetNoE();i++){
      read.SetSingleVariable(m_cbkg);
      if (int(read.GetInputSingle(i))==0) continue;
      unsigned cond_index=0;
      if (m_parameterized) param=param_vec[d(gen)];
      for (auto sp : m_split){
	read.SetSingleVariable(sp);
	int split_cond = int(read.GetInputSingle(i));
	read.SetSingleVariable(m_wbkg);
	Double_t weight = read.GetInputSingle(i);
	vector<Double_t> vars = read.GetInput(i);
	if (m_parameterized) vars.push_back(param);
	if (split_cond){
	  loaders[cond_index]->AddBackgroundTrainingEvent(vars,weight);
	  tmp_bkg_train++;
	  totw_bkg_train+=weight;
	  param_map_bkg[param]+=1;
	}
	else{
	  loaders[cond_index]->AddBackgroundTestEvent(vars,weight);
	  tmp_bkg_test++;
	  totw_bkg_test+=weight;
	  param_map_bkg[param]+=1;
	}
	cond_index++;
      }
    }
    tot_bkg_train+=tmp_bkg_train;
    tot_bkg_test+=tmp_bkg_test;
    cout << "Training events : " << tmp_bkg_train << endl;
    cout << "Test events : " << tmp_bkg_test << endl;
    f->Close();
    cout << "\n" <<endl;
  }

  cout << "BACKGROUND SAMPLE INFO:" << endl;
  cout << "Total training events: " << tot_bkg_train << ", weighted: " << totw_bkg_train << endl;
  cout << "Total test events:     " << tot_bkg_test << ", weighted: " << totw_bkg_test << endl;
   
  if (m_parameterized){
    cout << "\nParameterization:" << endl;
    unsigned total_bkg = tot_bkg_train+tot_bkg_test;
    for (auto const& x : param_map_bkg)
      {
	double param = x.first;
	double ratio = 1.0*((double)x.second/(double)total_bkg);
	cout << param
	     << ':' 
	     << x.second
	     << ':'
	     << ratio
	     << endl ;
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
  
