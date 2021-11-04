#include "MVAKit/Layer.hpp"

using namespace std;

Layer::Layer(const string name, const unsigned size):
  m_status(false)
{
  m_name = name;
  m_size = size;
  m_nodes = unique_ptr<op::vec>(new op::vec(size,0));
}

Layer::~Layer()
{}

void Layer::InitLayer()
{
  bool b_inputs = false;
  bool b_bias = false;
  bool b_weights = false;
  bool b_func = false;

  cout << "Initializing the layer " << m_name << endl;
  
  if (m_inputs != NULL) {
    cout << "Inputs are set at " << &m_inputs << endl;
    b_inputs = true;
    m_ii = m_inputs->size();
  }
  else {
    cout << "Inputs are not set" << endl;
  }
  if (m_bias != NULL) {
    cout << "Bias are set at " << &m_bias << endl;
    b_bias = true;
    m_bj = m_bias->size();
  }
  else {
    cout << "Bias are not set" << endl;
  }
  if (m_weights != NULL) {
    cout << "Weights are set at " << &m_weights << endl;
    b_weights = true;
    m_wi = m_weights->size();
    m_wj = m_weights->at(0).size();
  }
  else {
    cout << "Weights are not set" << endl;
  }
  if (m_fnc != NULL) {
    cout << "Activation is set at " << &m_fnc << endl;
    b_func = true;
  }
  else {
    cout << "Activation is not set" << endl;
  }

  m_status = b_inputs && b_bias && b_weights && b_func;

  if (m_status){
    if (m_wi != m_ii) {
      cout << "Your layer " << m_name << " has dimension problem." << endl;
      cout << "Weight for each input " << m_wi << " must be equal to number of inputs" << m_ii << endl;
      exit(0);
    }
    if (m_wj != m_size || m_bj != m_size) {
      cout << "Your layer " << m_name << " has dimension problem." << endl;
      cout << "Weight for each output " << m_wj << " must be equal to node number" << m_size << endl;
      cout << "Bias for each output " << m_bj << " must be equal to node number" << m_size << endl;
      exit(0);
    }

    cout << "You have successfully created your layer " << m_name << endl;
    cout << "Dimension of layer is " << m_ii << " | " << m_wi << "x" << m_wj << " | " << m_size << endl;
  }
  else {
    cout << "Your layer " << m_name << " is not completed, please complete it then run InitLayer()." << endl;
    exit(0);
  }
}


void Layer::Eval()
{
  if (!m_status) {
    cout << "Your layer " << m_name << " is not completed, please complete it then run InitLayer()." << endl;
    exit(0);
  }

  for (unsigned j=0;j<m_wj;j++) {
    float sum=0.0;
    for (unsigned i=0;i<m_wi;i++) {
      sum += m_weights->at(i)[j]*m_inputs->at(i);
    }
    m_nodes->at(j) = m_fnc(sum+m_bias->at(j));
  }
  
}
