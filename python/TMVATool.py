from ctypes import *
from TMVAFunc import *
import os

class TMVATool(object):
    def __init__(self,name='Training'):
        cpp_lib = os.environ['TMVATOOL_HOME']+'/build/lib/libTMVATool.so'
        if (os.path.exists(cpp_lib)):
            print('Succesfully found your C++ library:',cpp_lib)
        else:
            print('Cannot fing your C++ library. Please check if TMVATool has been built')
            exit()
        self.__lib = cdll.LoadLibrary(cpp_lib)
        self.__lib.TMVATool_GetWeight.restype = c_float
        self.__lib.TMVATool_GetVars.restype = POINTER(c_float)
        self.__lib.TMVATool_GetSpectatorVars.restype = POINTER(c_float)
        self.__lib.TMVATool_GetLabel.restype = c_char_p
        self.__lib.TMVATool_GetLabelName.restype = c_char_p
        self.__lib.TMVATool_GetVariableName.restype = c_char_p
        self.__lib.TMVATool_GetSpectatorVariableName.restype = c_char_p
        self.__lib.TMVATool_GetParamName.restype = c_char_p
        self.__lib.TMVATool_GetArchitectureOpt.restype = c_char_p
        self.__lib.TMVATool_GetEngineOpt.restype = c_char_p
        self.__lib.TMVATool_GetLabelOpt.restype = c_char_p
        self.__nsplit = 0
        self.__nlabel = 0
        self.NVar = 0
        self.NVarSpec = 0
        self.LabelNames = []
        self.Variables = []
        self.SpecVariables = []
        self.__obj = self.__lib.TMVATool_new(c_char_p(name))

    def ReadConf(self):
        self.__lib.TMVATool_ReadConf(self.__obj)
        self.NVar = self.GetNVar()
        self.NVarSpec = self.GetNSpectatorVar()
        self.__nsplit = self.GetNSplit()
        self.__nlabel = self.GetNLabel()
        for i in range(self.__nlabel):self.LabelNames.append(self.GetLabelName(i))
        for i in range(self.NVar):self.Variables.append(self.GetVariableName(i))
        for i in range(self.NVarSpec):self.SpecVariables.append(self.GetSpectatorVariableName(i))
        varParam = self.GetParamName()
        if varParam :
            self.Variables.append(varParam)
            self.NVar += 1

    def GetNVar(self):
        return self.__lib.TMVATool_GetNVar(self.__obj)

    def GetNSplit(self):
        return self.__lib.TMVATool_GetNSplit(self.__obj)

    def GetNLabel(self):
        return self.__lib.TMVATool_GetNLabel(self.__obj)

    def GetNSpectatorVar(self):
        return self.__lib.TMVATool_GetNSpectatorVar(self.__obj)
        
    def Parser(self,argc,argv):
        arg_p = (c_char_p * argc)()
        arg_p[:] = argv
        arg_pp = POINTER(c_char_p)(arg_p)
        self.__lib.TMVATool_Parser(self.__obj,c_int(argc),arg_pp)

    def SetEvents(self):
        self.__lib.TMVATool_SetEvents(self.__obj)

    def NextEvent(self,sample,index,minimum=0,maximum=-1):
        return self.__lib.TMVATool_NextEvent(self.__obj,c_char_p(sample),index,minimum,maximum)
        
    def GetArchitectureOpt(self):
        ptr=self.__lib.TMVATool_GetArchitectureOpt(self.__obj)
        return ptr
        
    def GetEngineOpt(self):
        ptr=self.__lib.TMVATool_GetEngineOpt(self.__obj)
        return ptr
        
    def GetLabelOpt(self):
        ptr=self.__lib.TMVATool_GetLabelOpt(self.__obj)
        return ptr
        
    def GetWeight(self):
        return self.__lib.TMVATool_GetWeight(self.__obj)
        
    def GetVars(self):
        ptr=self.__lib.TMVATool_GetVars(self.__obj)
        Vars=[ptr[i] for i in range(self.NVar)]
        return Vars
        
    def GetSpectatorVars(self):
        ptr=self.__lib.TMVATool_GetSpectatorVars(self.__obj)
        Vars=[ptr[i] for i in range(self.NVarSpec)]
        return Vars
        
    def GetLabel(self):
        ptr=self.__lib.TMVATool_GetLabel(self.__obj)
        return ptr
        
    def GetLabelName(self,index):
        ptr=self.__lib.TMVATool_GetLabelName(self.__obj,c_int(index))
        return ptr
        
    def GetVariableName(self,index):
        ptr=self.__lib.TMVATool_GetVariableName(self.__obj,c_int(index))
        return ptr
        
    def GetSpectatorVariableName(self,index):
        ptr=self.__lib.TMVATool_GetSpectatorVariableName(self.__obj,c_int(index))
        return ptr
        
    def GetParamName(self):
        ptr=self.__lib.TMVATool_GetParamName(self.__obj)
        return ptr

    def GetEvents(self,sample,index):
        x,xspec,y,w=[],[],[],[]

        while self.NextEvent(sample,index):
            x.append(self.GetVars())
            xspec.append(self.GetSpectatorVars())
            y.append(self.GetLabel())
            w.append(self.GetWeight())
        labelTag = Labeling(self.GetLabelOpt())
        ModifyLabel(y,labelTag)
        
        return x,xspec,y,w
