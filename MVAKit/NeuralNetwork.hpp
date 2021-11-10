#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <thread>

#include "MVAKit/Layer.hpp"
#include "MVAKit/MVAKit.hpp"


using namespace std;

class NeuralNetwork {

public:

  NeuralNetwork();
  ~NeuralNetwork();

  void SetInputsSize(unsigned size) {m_inputs_size=size;}
  void SetInputs(op::vec *inputs) {m_inputs=inputs;}

  float GetOutput(unsigned index=0);
  
  void ReadWeights(string file);
  
  void CreateNetwork(string weight,string cnf);
  void CreateNetwork(string opt);
  void FeedForward();

private:

  vector<pair<string,unsigned>> ParseOpt(string opt);

  //Network vars
  op::vec *m_inputs;
  
  vector<unique_ptr<op::vec>> m_bias;
  vector<unique_ptr<op::mat>> m_weights;
  vector<unique_ptr<Layer>> m_layers;
  unsigned m_layers_size;

  vector<string> m_inputs_str;
  unsigned m_inputs_size;

  //Config vars
  unique_ptr<MVAKit> m_tool;

  
};

#endif
