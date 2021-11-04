#include "MVAKit/Activation.hpp"

using namespace std;

map<string,Activation::func> Activation::Function = {
  {"relu",Relu},
  {"tanh",Tanh},
  {"sigmoid",Sigmoid},
  {"linear",Linear}
};

float Activation::Relu(const float x) {
  if (x>=0) {return x;}
  else {return 0.0;}
}

float Activation::Tanh(const float x) {
  return tanhf(x);
}

float Activation::Sigmoid(const float x) {
  return 1.0/(1.0+exp(-1.0*x));
}

float Activation::Linear(const float x) {
  return x;
}
