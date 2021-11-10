#ifndef TRANSFORMERS_HPP
#define TRANSFORMERS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <thread>

#include "MVAKit/Layer.hpp"

using namespace std;

class Transformers {

public:

  Transformers();
  Transformers(string name, string file);
  ~Transformers();

  void Load(string name, string file);
  void StdTransform(op::vec &input);

  op::vec *TransformedData() {return m_result.get();}

private:

  string m_name;
  unsigned m_input_size;
  op::vec m_std_mean;
  op::vec m_std_scale;

  unique_ptr<op::vec> m_result;
  
};

#endif
