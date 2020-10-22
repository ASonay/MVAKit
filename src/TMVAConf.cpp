#include "../inc/TMVAConf.hpp"


using namespace std;

TMVAConf::TMVAConf()
{}

TMVAConf::~TMVAConf()
{}

void
TMVAConf::Parser(int argc,char **argv, vector<string> &v1, vector<string> &v2, string &file)
{
  int index_1st=0;
  int index_2nd=0;
  int index_3th=0;
  int tot_file_1st=0;
  int tot_file_2nd=0;
  bool is_ntupSig=false;
  bool is_ntupBkg=false;
  
  for (int i=1;i<argc;i++){
    string check = argv[i];
    //cout << check << endl;
    if (check=="--ntupSig")
      {index_1st=i;is_ntupSig=true;is_ntupBkg=false;}
    else if (check=="--ntupBkg")
      {index_2nd=i;is_ntupSig=false;is_ntupBkg=true;}
    else if (check=="--ConfFile")
      {index_3th=i;is_ntupSig=false;is_ntupBkg=false;i++;}
    else if (check.find("root") != string::npos)
      {
	if (is_ntupSig) tot_file_1st++;
	if (is_ntupBkg) tot_file_2nd++;
      }
    else{
      cout << "Usage of the script :" << endl;
      cout << "./tmva_trainer --ntupSig <ntupSig.root> --ntupBkg <ntupBkg.root> --ConfFile <ConfFile.txt>" << endl;
      exit(0);
    }
  }

  if (index_1st==0||index_2nd==0){
    cout << "Usage of the script :" << endl;
    cout << "./tmva_trainer --ntupSig <ntupSig.root> --ntupBkg <ntupBkg.root> --ConfFile <ConfFile.txt>" << endl;
    exit(0);
  }

  for (int i=index_1st+1;i<index_1st+tot_file_1st+1;i++){
    v1.push_back(argv[i]);
  }

  for (int i=index_2nd+1;i<index_2nd+tot_file_2nd+1;i++){
    v2.push_back(argv[i]);
  }

  file = argv[index_3th+1];
  
}

void
TMVAConf::SetConf(vector<string> ntupsSig,vector<string> ntupsBkg,vector<string> variables,vector<string> split,string wbkg,string wsig,string cbkg,string csig){
  m_variables = variables;
  m_ntupsSig  = ntupsSig ;
  m_ntupsBkg  = ntupsBkg ;
  m_split     = split    ;
  m_wsig      = wsig     ;
  m_wbkg      = wbkg     ;
  m_csig      = csig     ;
  m_cbkg      = cbkg     ;
}

void
TMVAConf::SetEvents(const vector<TMVA::DataLoader*> &loaders){

  unsigned tot_sig_train=0,tot_sig_test=0;
  unsigned tot_bkg_train=0,tot_bkg_test=0;
  float totw_sig_train=0,totw_sig_test=0;
  float totw_bkg_train=0,totw_bkg_test=0;
  
  for (auto x: m_ntupsSig){
    cout << "File name to be readed : " << x << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      return;
    }
    cout << "Variables:" << endl;
    for (auto var : m_variables)
      cout << var << endl;
    TTree *tree = (TTree*) f->Get("nominal_Loose");
    ReadTree read(x,tree,m_variables);
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
	if (split_cond){
	  loaders[cond_index]->AddSignalTrainingEvent(vars,weight);
	  tot_sig_train++;
	  totw_sig_train+=weight;
	}
	else{
	  loaders[cond_index]->AddSignalTestEvent(vars,weight);
	  tot_sig_test++;
	  totw_sig_test+=weight;
	}
	cond_index++;
      }
      
    }
    f->Close();
    cout << "\n" <<endl;
  }

  cout << "SIGNAL SAMPLE INFO:" << endl;
  cout << "Total training events: " << tot_sig_train << ", weighted: " << totw_sig_train << endl;
  cout << "Total test events:     " << tot_sig_test << ", weighted: " << totw_sig_test << endl;

  
  for (auto x: m_ntupsBkg){
    cout << "File name to be readed : " << x << endl;
    TFile *f = new TFile(x.c_str());
    if (!f) {
      cout << "ERROR: cannot open file: " << x << endl;
      return;
    }
    cout << "Variables:" << endl;
    for (auto var : m_variables)
      cout << var << endl;
    TTree *tree = (TTree*) f->Get("nominal_Loose");
    ReadTree read(x,tree,m_variables);
    for (int i=0;i<read.GetNoE();i++){
      read.SetSingleVariable(m_cbkg);
      if (int(read.GetInputSingle(i))==0) continue;
      unsigned cond_index=0;
      for (auto sp : m_split){
	read.SetSingleVariable(sp);
	int split_cond = int(read.GetInputSingle(i));
	read.SetSingleVariable(m_wbkg);
	Double_t weight = read.GetInputSingle(i);
	vector<Double_t> vars = read.GetInput(i);
	if (split_cond){
	  loaders[cond_index]->AddBackgroundTrainingEvent(vars,weight);
	  tot_bkg_train++;
	  totw_bkg_train+=weight;
	}
	else{
	  loaders[cond_index]->AddBackgroundTestEvent(vars,weight);
	  tot_bkg_test++;
	  totw_bkg_test+=weight;
	}
	cond_index++;
      }
      
    }
    f->Close();
    cout << "\n" <<endl;
  }

  cout << "BACKGROUND SAMPLE INFO:" << endl;
  cout << "Total training events: " << tot_bkg_train << ", weighted: " << totw_bkg_train << endl;
  cout << "Total test events:     " << tot_bkg_test << ", weighted: " << totw_bkg_test << endl;

}
