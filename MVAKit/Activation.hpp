#ifndef ACTIVATION_HPP
#define ACTIVATION_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include <array>
#include <iostream>
#include <functional>
#include <map>
#include <math.h>

using namespace std;

namespace Activation {
  // Relu
  float Relu(const float x);
  //
  //
  // Tanh
  float Tanh(const float x);
  //
  //
  // Sigmoid
  float Sigmoid(const float x);
  //
  //
  // Linear
  float Linear(const float x);
  //
  //
  // define float function
  using func = float (*)(const float);
  //
  //
  // find activation
  extern map<string,Activation::func> Function;

}
#endif
