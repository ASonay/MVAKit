
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TSystem.h"
#include "TCut.h"
#include "TROOT.h"
#include "TStyle.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/PyMethodBase.h"


#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "MVAKit/MVAKit.hpp"

using namespace std;

int main(int argc,char **argv)
{
  // Configuration part-----------------------------
  auto tool = make_unique<MVAKit>("Training");

  tool->isClassification(0);
  tool->Parser(argc,argv);
  tool->ReadConf();
  tool->SetFile("data.root");
  tool->SetEvents();
  tool->CloseFile();
}
