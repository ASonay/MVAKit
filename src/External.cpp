#include "TSystem.h"
#include "TMVATool/TMVATool.hpp"

using namespace std;

extern "C" {
  TMVATool* TMVATool_new(const char* name){ return new TMVATool(name); }
  void TMVATool_ReadConf(TMVATool* tool){ tool->ReadConf(); }
  void TMVATool_Parser(TMVATool* tool,int argc,char **argv){ tool->Parser(argc,argv); }
  void TMVATool_SetEvents(TMVATool* tool){ tool->SetEvents(); }
  int TMVATool_NextEvent(TMVATool* tool,char *sample,int split_index,int start=0,int max=0){ return tool->NextEvent(sample,split_index,start,max); }
  float TMVATool_GetWeight(TMVATool* tool){ return tool->GetWeight(); }
  const char *TMVATool_GetLabel(TMVATool* tool){ return tool->GetLabel(); }
  float *TMVATool_GetVars(TMVATool* tool){ return tool->GetVars(); }
  float *TMVATool_GetSpectatorVars(TMVATool* tool){ return tool->GetSpectatorVars(); }
  int TMVATool_GetNVar(TMVATool* tool){ return tool->GetNVar(); }
  int TMVATool_GetNSpectatorVar(TMVATool* tool){ return tool->GetNSpectatorVar(); }
  
}
