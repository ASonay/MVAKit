#include "TSystem.h"
#include "MVAKit/MVAKit.hpp"
#include "TMVA/DataLoader.h"

using namespace std;

extern "C" {
  MVAKit* MVAKit_new(const char* name){ return new MVAKit(name); }
  void MVAKit_ReadConf(MVAKit* tool){ tool->ReadConf(); }
  void MVAKit_Parser(MVAKit* tool,int argc,char **argv,int reqntup){ tool->Parser(argc,argv,reqntup); }
  void MVAKit_SetEvents(MVAKit* tool,const char *file,const char *label,int doCut){ tool->SetEvents(file,label,doCut); }
  
  int MVAKit_GetNVar(MVAKit* tool){ return tool->GetNVar(); }
  int MVAKit_GetNSpectatorVar(MVAKit* tool){ return tool->GetNSpectatorVar(); }
  int MVAKit_GetNSplit(MVAKit* tool){ return tool->GetNSplit(); }
  int MVAKit_GetNLabel(MVAKit* tool){ return tool->GetNLabel(); } 

  void MVAKit_isExe(MVAKit* tool,int cond){ tool->isExe(cond); }
  void MVAKit_GetLabelName(MVAKit* tool,int index, char carr[]){ tool->GetLabelName(index,carr); }
  void MVAKit_GetArchitectureOpt(MVAKit* tool, char carr[]){ tool->GetArchitectureOpt(carr); }
  void MVAKit_GetEngineOpt(MVAKit* tool, char carr[]){ tool->GetEngineOpt(carr); }
  void MVAKit_GetLabelOpt(MVAKit* tool, char carr[]){ tool->GetLabelOpt(carr); }
  void MVAKit_GetVariableName(MVAKit* tool,int index, char carr[], int pair){ tool->GetVariableName(index,carr,pair); }
  void MVAKit_GetSpectatorVariableName(MVAKit* tool,int index, char carr[], int pair){ tool->GetSpectatorVariableName(index,carr,pair); }
  void MVAKit_GetParamName(MVAKit* tool, char carr[]){ tool->GetParamName(carr); }

  void MVAKit_SetLoaders(MVAKit* tool,TMVA::DataLoader *loader){ tool->SetLoaders(loader); }
  void MVAKit_SetFile(MVAKit* tool,const char *name){ tool->SetFile(name); }
  void MVAKit_SetConf(MVAKit* tool,const char *name){ tool->SetConf(name); }
  void MVAKit_CloseFile(MVAKit* tool){ tool->CloseFile(); }
  void MVAKit_SetCSV(MVAKit* tool,const char *name){ tool->SetCSV(name); }
  void MVAKit_CloseCSV(MVAKit* tool){ tool->CloseCSV(); }
  void MVAKit_isClassification(MVAKit* tool, int i){ tool->isClassification(i); }
  
}
