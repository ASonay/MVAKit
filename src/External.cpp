#include "TSystem.h"
#include "TMVATool/TMVATool.hpp"

using namespace std;

extern "C" {
  TMVATool* TMVATool_new(const char* name){ return new TMVATool(name); }
  void TMVATool_ReadConf(TMVATool* tool){ tool->ReadConf(); }
  void TMVATool_Parser(TMVATool* tool,int argc,char **argv){ tool->Parser(argc,argv); }
  void TMVATool_SetEvents(TMVATool* tool){ tool->SetEvents(); }
  void TMVATool_FreeMem(TMVATool* tool,void *ptr){ tool->FreeMem(ptr); }
  int TMVATool_NextEvent(TMVATool* tool,char *sample,int split_index,int start=0,int max=0){ return tool->NextEvent(sample,split_index,start,max); }
  double TMVATool_GetWeight(TMVATool* tool){ return tool->GetWeight(); }
  char *TMVATool_GetLabel(TMVATool* tool){ return tool->GetLabel(); }
  char *TMVATool_GetLabelName(TMVATool* tool,int index){ return tool->GetLabelName(index); }
  double *TMVATool_GetVars(TMVATool* tool){ return tool->GetVars(); }
  double *TMVATool_GetSpectatorVars(TMVATool* tool){ return tool->GetSpectatorVars(); }
  int TMVATool_GetNVar(TMVATool* tool){ return tool->GetNVar(); }
  int TMVATool_GetNSpectatorVar(TMVATool* tool){ return tool->GetNSpectatorVar(); }
  int TMVATool_GetNSplit(TMVATool* tool){ return tool->GetNSplit(); }
  int TMVATool_GetNLabel(TMVATool* tool){ return tool->GetNLabel(); }
  char *TMVATool_GetArchitectureOpt(TMVATool* tool){ return tool->GetArchitectureOpt(); }
  char *TMVATool_GetEngineOpt(TMVATool* tool){ return tool->GetEngineOpt(); }
  char *TMVATool_GetLabelOpt(TMVATool* tool){ return tool->GetLabelOpt(); }
  char *TMVATool_GetVariableName(TMVATool* tool,int index){ return tool->GetVariableName(index); }
  char *TMVATool_GetSpectatorVariableName(TMVATool* tool,int index){ return tool->GetSpectatorVariableName(index); }
  char *TMVATool_GetParamName(TMVATool* tool){ return tool->GetParamName(); }
  
}
