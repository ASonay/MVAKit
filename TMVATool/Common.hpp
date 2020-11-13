#ifndef COMMON_HPP
#define COMMON_HPP

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


using namespace std;

namespace Common {
  // Removing the special characters
  string RemoveSpecialChars(const string str);
  //
  //
  // String separation by char and vectorize
  vector<string> StringSep(const string str,char c=',');
  //
  //
  // String compare regardless case
  bool StringCompare(const string str1, const string str2);
  //
  //
  // String find regardless case
  bool StringFind(const string str1, const string str2);
  //
  //
  // Check if string digit
  bool CheckDigit(const string str);
  //
  //
  // Find digit in string
  double FindDigit(const string ref,const string var);

}
#endif
