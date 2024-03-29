#include "TSystem.h"
#include "TROOT.h"

#include "MVAKit/Configuration.hpp"


using namespace std;

Configuration::Configuration(const char *name):
  m_paramFile(""),
  m_psplit("Cross-section"),
  m_split_per(0),
  m_parameterized(false),
  m_classification(true),
  m_execution(true),
  m_importance(false),
  m_ntuplefilled(false)
{
  m_confName = string(name);
  cout << "Configuration will be setup for " << m_confName << ".\n" << endl;
  m_trainingOpt = "!H:!V:NTrees=600:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.6:SeparationType=GiniIndex:nCuts=30:MaxDepth=2:NegWeightTreatment=IgnoreNegWeightsInTraining";
  m_factoryOpt = "!V:!Silent:Transformations=I;D;P;G,D:AnalysisType=Classification";
  m_samplingOpt = "nTrain_Signal=0:nTrain_Background=0:NormMode=EqualNumEvents:V";
  m_classifierOpt = "BDT";
}

Configuration::~Configuration()
{}

void
Configuration::Parser(int argc,char **argv,int reqntup)
{
  if (!reqntup) {m_ntuplefilled=true;}

  string app_name = argv[0];
  string command;
  if (m_confName=="Training")
    {command = app_name+" --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --ConfFile <ConfFile.txt>";}
  else if (m_confName=="Evaluate")
    {command = app_name+" --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --ConfFile <ConfFile.txt>";}
  else
    {
      cout << "Could not find any proper configuration name." << endl;
      cout << "'Training' or 'Evaluate' should be." << endl;
      exit(0);
    }
  
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
	      m_labels.push_back(check);
	      i=j;
	      break;
	    }
	}
      }
    else if (check=="--par")
      {m_param = argv[i+1];i++;}
    else if (check=="--var")
      {m_cname = argv[i+1];i++;}
    else if ((check=="--ConfFile" || check=="--conf") && !is_file)
      {m_confFile = argv[i+1];is_file=true;i++;}
    else{
      cout << "Usage of the script :" << endl;
      cout << "Check here, " << check << endl;
      cout << command << endl;
      exit(0);
    }
  }

  if (!is_ntup && reqntup){
    cout << "Usage of the script :" << endl;
    cout << command << endl;
    exit(0);
  }

  if (!is_file){
    cout << "Usage of the script :" << endl;
    cout << command << endl;
    exit(0);
  }

}

void
Configuration::ReadConf(){
  ifstream in(m_confFile);
  string str;

  if (in.fail()) {cout << "Can not find config file : " << m_confFile << "\n"; exit(0);}
  cout << "\nStart reading configuration file : " << m_confFile << endl;
  while (in >> str){

    if (str[0]=='%' || str[0]=='#') {
      in >> str;
      continue;
    }
    
    if (str.compare("Var:")==0){
      in >> str;
      vector<string> line_with_type = Common::StringSep(str,'\\');
      string type = "flat";
      if (line_with_type.size()>1) {type = line_with_type[1];}
      if (type == "flat" && Common::StringSep(str,';').size()>1) {
	cout << "Multiple variables in same line cannot be flat" << endl;
	cout << str << endl;
	exit(0);
      }
      vector<string> line = Common::StringSep(line_with_type[0],'@');
      if (line.size() == 0 && Common::StringSep(str,';').size()>1) {
	cout << "If multiple variables used in same line, variable name must be declared" << endl;
	cout << str << endl;
	exit(0);	
      }
      if (line.size()>1)
	{m_variables.push_back({line[0],Common::RemoveSpecialChars(line[1]),type});}
      else
	{m_variables.push_back({line[0],Common::RemoveSpecialChars(line[0]),type});}
    }
    else if (str.compare("OtherVar:")==0){
      in >> str;
      vector<string> line_with_type = Common::StringSep(str,'\\');
      string type = "flat";
      if (line_with_type.size()>1) {type = line_with_type[1];}
      if (type == "flat" && Common::StringSep(str,';').size()>1) {
	cout << "Multiple variables in same line cannot be flat" << endl;
	cout << str << endl;
	exit(0);
      }
      vector<string> line = Common::StringSep(line_with_type[0],'@');
      if (line.size() == 0 && Common::StringSep(str,';').size()>1) {
	cout << "If multiple variables used in same line, variable name must be declared" << endl;
	cout << str << endl;
	exit(0);	
      }
      if (line.size()>1)
	{m_variables_other.push_back({line[0],Common::RemoveSpecialChars(line[1]),type});}
      else
	{m_variables_other.push_back({line[0],Common::RemoveSpecialChars(line[0]),type});}
    }
    else if (str.compare("ParamVar:")==0){
      in >> str;
      m_paramvar=str;
    }
    else if (str.find("Scale:")!=string::npos && str.find("ParamScale:")==string::npos){
      auto strPos = str.find("Scale:");
      string label = str.substr(0,strPos);
      in >> str;
      map<string,double> scale_map = ScaleMap(str);
      m_scale.push_back(make_pair(label,scale_map));
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
    else if (str.find("Tree:")!=string::npos){
      auto strPos = str.find("Tree:");
      string label = str.substr(0,strPos);
      in >> str;
      m_tree.push_back(make_pair(label,str));
    }
    else if (str.find("AddAlias:")!=string::npos){
      in >> str;
      auto alias = Common::StringSep(str,';');
      if (alias.size()==2) {
	m_alias.push_back(make_pair(alias[0],alias[1]));
      }
      else {
	cout << "AddAlias method require following method, alias;formula" << endl;
	cout << "Yours has been failed for " << str << endl;
	exit(0);
      }
    }
    else if (str.compare("TrainingOpt:")==0){
      in >> str;
      m_trainingOpt=str;
    }
    else if (str.compare("ArchitectureOpt:")==0){
      in >> str;
      m_arcOpt=str;
    }
    else if (str.compare("FactoryOpt:")==0){
      in >> str;
      m_factoryOpt=str;
    }
    else if (str.compare("ClassifierOpt:")==0){
      in >> str;
      m_classifierOpt=str;
    }
    else if (str.compare("EngineOpt:")==0){
      in >> str;
      m_engOpt=str;
    }
    else if (str.compare("SamplingOpt:")==0){
      in >> str;
      m_samplingOpt=str;
    }
    else if (str.compare("LabelOpt:")==0){
      in >> str;
      m_labOpt=str;
    }
    else if (str.compare("Split:")==0){
      in >> str;
      m_split=Common::StringSep(str);
    }
    else if (str.compare("ParamSplit:")==0){
      in >> str;
      m_psplit=str;
    }
    else if (str.compare("ParamScale:")==0){
      in >> str;
      m_pscale=str;
    }
    else if (str.compare("ParamFiles:")==0){
      in >> str;
      m_paramFile=str;
    }
    else if (str.compare("EvalImportance:")==0){
      in >> str;
      if (Common::StringCompare(str,"true")){
	m_importance=true;
      }
    }
    else if (str.compare("LoadFiles:")==0){
      in >> str;
      m_loadFile=Common::StringSep(str);
    }
    else if (str.compare("ExecuteFunctions:")==0){
      in >> str;
      m_exe=Common::StringSep(str,';');
    }
    else if (str.compare("LoadLibs:")==0){
      in >> str;
      m_loadLib=Common::StringSep(str);
    }
    else{
      cout << "Your config file is badly formatted.." << endl;
      cout << "Check here: " << str << endl;
      exit(0);
    }
  }
  in.close();
  cout << "\nFinished to read configuration..\n" << endl;

  m_nlabel = m_labels.size();
  m_nvar=m_variables.size();
  m_nvarSpec=m_variables_other.size();
  m_nsplit=m_split.size();

  if (m_execution){
    for (auto x : m_loadLib){
      int libLoad = gSystem->Load(x.c_str());
      if ( libLoad==0)
	{cout << "Your library, " << x << " successfully loaded." << endl;}
      else if ( libLoad==1)
	{cout << "Your library, " << x << " successfully loaded before." << endl;}
      else{
	cout << "Your library, " << x << " cannot be loaded." << endl;
	exit(0);
      }
    }

    for (auto x : m_loadFile){
      if (gROOT->LoadMacro((x+"+").c_str())==0)
	{cout << "Your macro, " << x << " successfully loaded." << endl;}
      else{
	cout << "Your macro, " << x << " cannot be loaded." << endl;
	exit(0);
      }
    }

    for (auto x : m_exe){
      gROOT->ProcessLine(x.c_str());
    }
  }
  
  if (m_nsplit==0)
    m_split.push_back("Split;50");

  if (m_split[0].find("Split;") != string::npos)
    {m_split_per = atoi(m_split[0].substr(6).c_str());}

  if (!m_paramvar.empty()){
    cout << "\nParameterization detected for : " << m_paramvar << endl;
    m_parameterized=true;
  }
    
  if (m_nlabel<2 && m_classification){
    cout << "You should at least have two different label for classification." << endl;
    exit(0);
  }

  if (m_importance){
    cout << "\nIMPORTANCE will be calculated, hope this is TMVA..\n" << endl;
  }

  if (m_parameterized && m_xmlFile.empty() && !m_ntuplefilled && m_paramFile.empty()){
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

  if (!m_paramFile.empty() && m_xmlFile.empty() && !m_ntuplefilled){
    cout << "\nParameterization detected for file selection " << m_paramFile << endl;
    m_parameterized = true;
    if (m_paramvar.empty()) {m_paramvar="Param";}
    const int columnId = 0;
    const vector<string> paramFind = Common::StringSep(m_paramFile,',');
    sort(m_ntups.begin(), m_ntups.end(),
	 [&columnId,&paramFind](const pair<string,vector<string>> &a, const pair<string,vector<string>> &b) 
	 {
	   return Common::IsIn(a.second[columnId],paramFind) && !Common::IsIn(b.second[columnId],paramFind);
	 });
  }
}

vector<TString>
Configuration::GetTVariables(){
  vector<TString> newvars;
  for (auto const& var : m_variables){
    newvars.push_back(var.second);
  }

  if (!m_paramvar.empty()){
    newvars.push_back(m_paramvar);
  }
  
  return newvars;
}

vector<TString>
Configuration::GetTVariablesOther(){
  vector<TString> newvars;
  for (auto const& var : m_variables_other){
    newvars.push_back(var.second);
  }
  return newvars;
}

vector<string>
Configuration::GetVariables(){
  vector<string> newvars;
  for (auto const& var : m_variables){
    newvars.push_back(var.first);
  }

  if (!m_paramvar.empty()){
    newvars.push_back(m_paramvar);
  }
  
  return newvars;
}

vector<string>
Configuration::GetVariablesOther(){
  vector<string> newvars;
  for (auto const& var : m_variables_other){
    newvars.push_back(var.first);
  }
  return newvars;
}

vector<TString>
Configuration::GetTXML(){
  vector<TString> newxml;
  for (auto x : m_xmlFile) newxml.push_back(x);
  return newxml;
}

map<string,double>
Configuration::ScaleMap(const string str){
  map<string,double> result;
  vector<string> getlines;

  if (!str.empty())
    {getlines = Common::StringSep(str);}
  
  for (auto line : getlines){
    vector<string> map_values = Common::StringSep(line,';');
    if (map_values.size()==2){
      string key = map_values[0];
      double value = atof(map_values[1].c_str());
      result[key] = value;
    }
    else if (map_values.size()==1){
      double value = atof(map_values[0].c_str());
      if (value>0)
	{result["single"]=value;}
      else
	{cout << "Scaling value should be none zero but found : " << value << endl;}
    }
    else{
      cout << "Your scaling format doesn't match with any ->" << str << endl;
      cout << "For single sample scaling : <label>Scale: <value>" << endl;
      cout << "For scaling each MP : <label>Scale: <MP1>;<value1>,..,<MPN>;<valueN>" << endl;
      exit(0);
    }
  }
  
  return result;
}
