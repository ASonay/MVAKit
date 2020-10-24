#include <iomanip>

#include "../inc/ReadTree.hpp"

using namespace std;

ReadTree::ReadTree(string filoc, string finame, string tname, vector<string> var):
  tree(NULL),
  tf_s(NULL),
  formx(NULL)
{
  variables = var;
  name = finame;
  
  fi = new TFile( (filoc+finame+".root").c_str() );
  
  tree = (TTree*)fi->Get(tname.c_str());
  
  m_NoE = tree->GetEntries();

  tf.clear();
  for (int i=0;i<(int)variables.size();i++){
    formx = new TTreeFormula((name+"_"+to_string(i)).c_str(),variables[i].c_str(),tree);
    tf.push_back(formx);
  }

}

ReadTree::ReadTree(string reader_name, TTree *tr, vector<string> var):
  tree(NULL),
  tf_s(NULL),
  formx(NULL)
{
  variables = var;
  name = reader_name;
  
  tree = tr;
  
  m_NoE = tree->GetEntries();

  tf.clear();
  for (int i=0;i<(int)variables.size();i++){
    formx = new TTreeFormula((name+"_"+to_string(i)).c_str(),variables[i].c_str(),tree);
    tf.push_back(formx);
  }
 
}

ReadTree::~ReadTree(){
}

vector<double> ReadTree::GetInput(int n=0){

  xv.clear();
  tree->GetEntry(n);
  
  for (int i=0;i<(int)variables.size();i++){
    tf[i]->GetNdata();
    xv.push_back(tf[i]->EvalInstance());
  }


  return xv;
}

NormFactors ReadTree::GetNormFactors(int n){
  NormFactors x;
  vector<double> sum1(variables.size(),0.0);
  vector<double> sum2(variables.size(),0.0);
  vector<double> var;
 
  for (int i=0;i<n;i++){
    var=GetInput(i);
    for (int j=0;j<(int)variables.size();j++){
      sum1[j] += var[j];
      sum2[j] += var[j]*var[j];
    }
  }

  for (int j=0;j<(int)variables.size();j++){
    x.push_back(NormVars());
    x.back().avg = sum1[j]/n;
    x.back().sig = sqrt(sum2[j]/n);
  }

  cout << "-----------------------------------------Variables-------Average---------Sigma" << endl;
  for (int i=0;i<(int)variables.size();i++)
    cout << setw(50) << variables[i] << "  "
	 << setw(12) << x[i].avg << "  "
	 << setw(12) << x[i].sig << endl;
  cout << "------------------------------------------------------------------------------\n" << endl;

  return x;
}


void ReadTree::SetSingleVariable(string variable){
  delete tf_s;
  tf_s = new TTreeFormula(name.c_str(),variable.c_str(),tree);
}

int ReadTree::GetConditionalIndex(string cond, int beg){
  TTreeFormula *tf_c = new TTreeFormula((name+to_string(beg)).c_str(),cond.c_str(),tree);
  int index = -1;
  
  for (int i=beg;i<GetNoE();i++){
    tree->GetEntry(i);
    if (tf_c->EvalInstance())
      {index = i; break;}
  }

  delete tf_c;
  return index;
}

double ReadTree::GetInputSingle(int n=0){
  tree->GetEntry(n);
  xd = tf_s->EvalInstance();
  return xd;
}

void ReadTree::Close(){
  delete tree;
  delete formx;
  delete tf_s;
}
