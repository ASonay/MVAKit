#include "TSystem.h"
#include "TMVATool/TMVATool.hpp"

using namespace std;

extern "C" {
  TMVATool* TMVATool_new(const char* name){ return new TMVATool(name); }
  void TMVATool_ReadConf(TMVATool* tool){ tool->ReadConf(); }
  void TMVATool_Parser(TMVATool* tool,int argc,char **argv){ tool->Parser(argc,argv); }
  void TMVATool_SetEvents(TMVATool* tool){ tool->SetEvents(); }
  
  int TMVATool_PassEvent(TMVATool* tool,char *sample,int split_index,char label[],double &w, double vars[], double varsSpec[]){ return tool->PassEvent(sample,split_index,label,w,vars,varsSpec); }
  int TMVATool_GetNVar(TMVATool* tool){ return tool->GetNVar(); }
  int TMVATool_GetNSpectatorVar(TMVATool* tool){ return tool->GetNSpectatorVar(); }
  int TMVATool_GetNSplit(TMVATool* tool){ return tool->GetNSplit(); }
  int TMVATool_GetNLabel(TMVATool* tool){ return tool->GetNLabel(); }
  
  void TMVATool_GetLabelName(TMVATool* tool,int index, char carr[]){ tool->GetLabelName(index,carr); }
  void TMVATool_GetArchitectureOpt(TMVATool* tool, char carr[]){ tool->GetArchitectureOpt(carr); }
  void TMVATool_GetEngineOpt(TMVATool* tool, char carr[]){ tool->GetEngineOpt(carr); }
  void TMVATool_GetLabelOpt(TMVATool* tool, char carr[]){ tool->GetLabelOpt(carr); }
  void TMVATool_GetVariableName(TMVATool* tool,int index, char carr[]){ tool->GetVariableName(index,carr); }
  void TMVATool_GetSpectatorVariableName(TMVATool* tool,int index, char carr[]){ tool->GetSpectatorVariableName(index,carr); }
  void TMVATool_GetParamName(TMVATool* tool, char carr[]){ tool->GetParamName(carr); }
  
}
