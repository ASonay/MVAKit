#include "TSystem.h"
#include "TROOT.h"

#include "TMVATool/TMVAConf.hpp"


using namespace std;

TMVAConf::TMVAConf(const string name):
  m_psplit("Cross-section")
{
  m_confName = name;
  cout << "Configuration will be setup for " << name << ".\n" << endl;
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

  string app_name = argv[0];
  string command;
  if (m_confName=="Training")
    {command = app_name+" --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --ConfFile <ConfFile.txt>";}
  else if (m_confName=="Evaluate")
    {command = app_name+" --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --xml <file1.xml>,..,<filen.xml> <cond1>,..,<condn> --ConfFile <ConfFile.txt>";}
  else
    {
      cout << "Could not find any proper configuration name." << endl;
      cout << "'Training' or 'Evaluate' should be." << endl;
      exit(0);
    }
  
  bool is_ntup=false;
  bool is_file=false;
  bool is_xml=false;

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
    else if (check=="--xml")
      {m_xmlFile = ComaSep(argv[i+1]);m_cond = ComaSep(argv[i+2]);is_xml=true;i+=2;}
    else if (check=="--par")
      {m_param = argv[i+1];i++;}
    else if (check=="--var")
      {m_cname = argv[i+1];i++;}
    else if ((check=="--ConfFile" || check=="--conf") && !is_file)
      {m_confFile = argv[i+1];is_file=true;i++;}
    else{
      cout << "Usage of the script :" << endl;
      cout << command << endl;
      exit(0);
    }
  }

  if (!is_ntup){
    cout << "Usage of the script :" << endl;
    cout << command << endl;
    exit(0);
  }

  if (!is_file){
    cout << "Usage of the script :" << endl;
    cout << command << endl;
    exit(0);
  }

  if (!is_xml && m_confName=="Evaluate"){
    cout << "Usage of the script :" << endl;
    cout << command << endl;
    exit(0);
  }

  cout << "\nConfiguration File: " << m_confFile << endl;
}

void
TMVAConf::ReadConf(){
  ifstream in(m_confFile);
  string str;

  if (in.fail()) {cout << "Can not find config file : " << m_confFile << "\n"; exit(0);}
  
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
    else if (str.find("Tree:")!=string::npos){
      auto strPos = str.find("Tree:");
      string label = str.substr(0,strPos);
      in >> str;
      m_tree.push_back(make_pair(label,str));
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
      m_split=ComaSep(str);
    }
    else if (str.compare("ParamSplit:")==0){
      in >> str;
      m_psplit=str;
    }
    else if (str.compare("LoadFiles:")==0){
      in >> str;
      m_loadFile=ComaSep(str);
    }
    else if (str.compare("LoadLibs:")==0){
      in >> str;
      m_loadLib=ComaSep(str);
    }
    else{
      cout << "Your config file is badly formatted.." << endl;
      cout << "Check here: " << str << endl;
      exit(0);
    }
  }

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

}

vector<string>
TMVAConf::ComaSep(const string str){
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

vector<TString>
TMVAConf::GetTXML(){
  vector<TString> newxml;
  for (auto x : m_xmlFile) newxml.push_back(x);
  return newxml;
}
