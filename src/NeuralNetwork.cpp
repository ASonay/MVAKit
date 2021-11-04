#include "MVAKit/Common.hpp"
#include "MVAKit/Activation.hpp"
#include "MVAKit/NeuralNetwork.hpp"

using namespace std;

NeuralNetwork::NeuralNetwork():
  m_inputs_size(0)
{}

NeuralNetwork::NeuralNetwork(string weight,string cnf):
  m_inputs_size(0)
{  

  // Prepare MVA tool for config
  m_tool.reset(new MVAKit("Evaluate"));
  m_tool->isClassification(0);
  m_tool->isExe(0);
  m_tool->SetConf(cnf);
  m_tool->ReadConf();

  // Inputs & size
  m_inputs_str = m_tool->GetVariables();
  m_inputs_size = m_inputs_str.size();

  //Read weights and biasses from file.
  ReadWeights(weight);
  
  //Get Architecture
  char c_arch[1000];
  m_tool->GetArchitectureOpt(c_arch);
  string arch = string(c_arch);

  // Create Network
  CreateNetwork(arch);

  //Print work directory and config file
  cout << "Weight file: " << weight << endl;
  cout << "Config file will be readed: " << cnf << "\n"<< endl;
}

NeuralNetwork::~NeuralNetwork()
{
  m_bias.clear();
  m_weights.clear();
  m_layers.clear();
}

void NeuralNetwork::ReadWeights(string file)
{
  ifstream in(file);
  if (in.fail()) {cout << "Can not find file : " << file << "\n"; exit(0);}

  string str;
  int index;

  op::mat weights;
  op::vec bias;

  cout << "Opening file " << file << endl;
  while (in >> str){
    if (str == "layer") {
      in >> index;
      cout << "Reading layer " << index << endl;
      getline(in, str);
      getline(in, str);
      weights.clear();
      while (getline(in, str)){
	if (str!="Bias"){
	  weights.push_back(Common::StringSepFloat(str.substr(0, str.size()-1),' '));
	}
	else {
	  getline(in, str);
	  bias = Common::StringSepFloat(str.substr(0, str.size()-1),' ');
	  getline(in, str);
	  getline(in, str);
	  break;
	}
      }
      auto w_cont = make_unique<op::mat>(weights);
      auto b_cont = make_unique<op::vec>(bias);
      m_weights.push_back(move(w_cont));
      m_bias.push_back(move(b_cont));
    }
  }
  in.close();
  cout << "Closing file..\n" << endl;
  m_layers_size = index+1;
}

void NeuralNetwork::CreateNetwork(string opt)
{
  auto pars=ParseOpt(opt);

  //Check layer size
  if (pars.size() != m_layers_size) {
    cout << "Parser found " << pars.size() << " layers but from the weights " << m_layers_size << " layers read." << endl;
    exit(0);
  }
  
  m_inputs.reset(new op::vec(m_inputs_size,1.0));

  int count=0;
  cout << "Creating the network..." << endl;
  for (auto par : pars) {
    cout << "Layer" << count << "(" << par.first << ")" << "[" << par.second << "]" << endl;
    auto layer = make_unique<Layer>(par.first+to_string(count),par.second);
    layer->SetActivation(Activation::Function[Common::LowerCase(par.first)]);
    if (count==0) {layer->SetInputs(m_inputs.get());}
    else {layer->SetInputs(m_layers.back()->GetNodes());}
    layer->SetBias(m_bias[count].get());
    layer->SetWeights(m_weights[count].get());
    layer->InitLayer();
    m_layers.push_back(move(layer));
    count++;
  }
  cout << "Network created...\n" << endl;

}


vector<pair<string,unsigned>> NeuralNetwork::ParseOpt(string opt)
{
  if (m_inputs_size==0) {
    cout << "Inputs size must be decclared before creating a network, by ->SetInputsSize(size)." << endl;
    exit(0);
  }

  cout << "Parsing your architecture from str, " << opt << "\n" << endl;
  
  vector<pair<string,unsigned>> parsed;
  
  vector<string> layers = Common::StringSep(opt);
  
  for (auto l : layers) {
    vector<string> layer = Common::StringSep(l,'|');
    if (layer.size()<2) {
      cout << "Your NN configuration formatted wrong." << endl;
      exit(0);
    }
    else {
      parsed.push_back(make_pair(layer[0],atoi(layer[1].c_str())));
    }
  }

  if (parsed.size() == 0) {
    cout << "No option found for NN architecture." << endl;
    cout << "Please be ensure you declared a valid option, e.g. relu|30|he_uniform,tanh|30,sigmoid|1" << endl;
    exit(0);
  }

  return parsed;
}

void NeuralNetwork::SetInputs(op::vec inputs)
{
  for (unsigned i=0;i<m_inputs_size;i++) {
    m_inputs->at(i) = inputs[i];
  }
}

void NeuralNetwork::FeedForward()
{
  for (auto &layer : m_layers) {
    layer->Eval();
  }
}

float NeuralNetwork::GetOutput(unsigned index)
{
  return m_layers.back()->GetNodes()->at(index);
}
