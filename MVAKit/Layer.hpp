#ifndef LAYER_HPP
#define LAYER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <thread>

using namespace std;

namespace op {
  typedef std::vector<float> vec;
  typedef std::vector<vec> mat;
}

class Layer {

public:

  Layer(const string name, const unsigned size);
  ~Layer();

  void SetActivation(float (*fnc)(const float)) {m_fnc=fnc;}
  void SetInputs(op::vec *inputs) {m_inputs=inputs;}
  void SetBias(op::vec *bias) {m_bias=bias;}
  void SetWeights(op::mat *weights) {m_weights=weights;}
  void InitLayer();

  op::vec *GetNodes() {return m_nodes.get();}

  void Eval();

private:

  string m_name;
  
  float (*m_fnc)(const float);
  
  op::vec *m_inputs;
  op::vec *m_bias;
  op::mat *m_weights;

  unique_ptr<op::vec> m_nodes;

  unsigned m_size;
  unsigned m_wi;
  unsigned m_wj;
  unsigned m_ii;
  unsigned m_bj;

  bool m_status;
};

#endif
