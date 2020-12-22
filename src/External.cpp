#include "TSystem.h"
#include "MVAKit/MVAKit.hpp"
#include "TMVA/DataLoader.h"

using namespace std;

extern "C" {
  MVAKit* MVAKit_new(const char* name){ return new MVAKit(name); }
  void MVAKit_ReadConf(MVAKit* tool){ tool->ReadConf(); }
  void MVAKit_Parser(MVAKit* tool,int argc,char **argv){ tool->Parser(argc,argv); }
  void MVAKit_SetEvents(MVAKit* tool){ tool->SetEvents(); }
  
  int MVAKit_GetNVar(MVAKit* tool){ return tool->GetNVar(); }
  int MVAKit_GetNSpectatorVar(MVAKit* tool){ return tool->GetNSpectatorVar(); }
  int MVAKit_GetNSplit(MVAKit* tool){ return tool->GetNSplit(); }
  int MVAKit_GetNLabel(MVAKit* tool){ return tool->GetNLabel(); }
  
  void MVAKit_GetLabelName(MVAKit* tool,int index, char carr[]){ tool->GetLabelName(index,carr); }
  void MVAKit_GetArchitectureOpt(MVAKit* tool, char carr[]){ tool->GetArchitectureOpt(carr); }
  void MVAKit_GetEngineOpt(MVAKit* tool, char carr[]){ tool->GetEngineOpt(carr); }
  void MVAKit_GetLabelOpt(MVAKit* tool, char carr[]){ tool->GetLabelOpt(carr); }
  void MVAKit_GetVariableName(MVAKit* tool,int index, char carr[]){ tool->GetVariableName(index,carr); }
  void MVAKit_GetSpectatorVariableName(MVAKit* tool,int index, char carr[]){ tool->GetSpectatorVariableName(index,carr); }
  void MVAKit_GetParamName(MVAKit* tool, char carr[]){ tool->GetParamName(carr); }

  void MVAKit_SetLoaders(MVAKit* tool,TMVA::DataLoader *loader){ tool->SetLoaders(loader); }
  void MVAKit_SetFile(MVAKit* tool,const char *name){ tool->SetFile(name); }
  void MVAKit_CloseFile(MVAKit* tool){ tool->CloseFile(); }
  void MVAKit_isClassification(MVAKit* tool, int i){ tool->isClassification(i); }
  
}
