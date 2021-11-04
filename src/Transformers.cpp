#include "MVAKit/Transformers.hpp"
#include "MVAKit/Common.hpp"

using namespace std;

Transformers::Transformers():
  m_name(""),
  m_input_size(0)
{}

Transformers::Transformers(string name, string file):
  m_name(""),
  m_input_size(0)
{  
  Load(name,file);
}

Transformers::~Transformers()
{}

void Transformers::Load(string name, string file)
{
  ifstream in(file);
  if (in.fail()) {cout << "Can not find file : " << file << "\n"; exit(0);}

  m_name=name;

  string str;
  float val1,val2;

  cout << "Opening file " << file << endl;
  cout << "Variable -----|----- Mean -----|----- Sigma " << endl;
  while (in >> str >> val1 >> val2){
    cout << str << " " << val1 << " " << val2 << endl;
    m_std_mean.push_back(val1);
    m_std_scale.push_back(val2);
  }
  in.close();
  cout << "Closing file..\n" << endl;

  m_input_size = m_std_mean.size();
}

void Transformers::StdTransform(op::vec &input)
{
  for (unsigned i=0;i<m_input_size;i++){
    input[i]=(input[i]-m_std_mean[i])/m_std_scale[i];
  }
}
