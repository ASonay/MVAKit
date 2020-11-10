#include "TMVATool/Common.hpp"

using namespace std;

vector<string>
Common::StringSep(const string str,char c){
  stringstream ss( str );
  vector<string> result;

  while( ss.good() )
    {
      string substr;
      getline( ss, substr, c );
      result.push_back( substr );
    }

  return result;
}

string
Common::RemoveSpecialChars(const string str){
  string str_r(str);
  replace( str_r.begin(), str_r.end(), '<', 's');
  replace( str_r.begin(), str_r.end(), '>', 'g');
  replace( str_r.begin(), str_r.end(), '*', '_');
  replace( str_r.begin(), str_r.end(), '=', 'e');
  replace( str_r.begin(), str_r.end(), '+', '_');
  replace( str_r.begin(), str_r.end(), '-', '_');
  replace( str_r.begin(), str_r.end(), '.', '_');
  replace( str_r.begin(), str_r.end(), '&', '_');
  replace( str_r.begin(), str_r.end(), '*', '_');
  replace( str_r.begin(), str_r.end(), '{', '_');
  replace( str_r.begin(), str_r.end(), '}', '_');
  replace( str_r.begin(), str_r.end(), '(', '_');
  replace( str_r.begin(), str_r.end(), ')', '_');
  replace( str_r.begin(), str_r.end(), '[', '_');
  replace( str_r.begin(), str_r.end(), ']', '_');
  replace( str_r.begin(), str_r.end(), '$', '_');
  replace( str_r.begin(), str_r.end(), '/', '_');
  return str_r;
}

bool
Common::StringCompare(const string str1, const string str2)
{

  string a = str1; string b = str2;
  
  transform(a.begin(), a.end(), a.begin(),
		 [](unsigned char c){ return tolower(c); });
  transform(b.begin(), b.end(), b.begin(),
		 [](unsigned char c){ return tolower(c); });
  
  return a.find(b) != string::npos || b.find(a) != string::npos;
}

bool
Common::CheckDigit(const string str)
{
  unsigned char negsign = '-';
  bool isDigit=find_if(str.begin(), str.end(),
		       [&negsign](unsigned char c)
		       { return !isdigit(c) && negsign!=c; }) == str.end();

  if (isDigit){
    double val = atof(str.c_str());
    if (val<=0){
      cout << "Your value " << str << ", has to be positive and different than zero." << endl;
      exit(0);
    }
  }
  
  return isDigit;
}

double
Common::FindDigit(const string ref,const string var){
  auto strPos = ref.find(var);
  string str_tmp = ref.substr(strPos+var.length());
  double val = atoi(str_tmp.c_str());
  return val;
}
