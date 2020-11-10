from ctypes import *

class TMVATool(object):
    def __init__(self,name='Training'):
        self.__lib = cdll.LoadLibrary('../build/lib/libTMVATool.so')
        self.__lib.TMVATool_GetWeight.restype = c_float
        self.__lib.TMVATool_GetVars.restype = POINTER(c_float)
        self.__lib.TMVATool_GetSpectatorVars.restype = POINTER(c_float)
        self.__lib.TMVATool_GetLabel.restype = c_void_p
        self.__nvar = 0
        self.__nvarSpec = 0
        self.__obj = self.__lib.TMVATool_new(c_char_p(name))

    def ReadConf(self):
        self.__lib.TMVATool_ReadConf(self.__obj)
        self.__nvar = self.GetNVar()
        self.__nvarSpec = self.GetNSpectatorVar()

    def GetNVar(self):
        return self.__lib.TMVATool_GetNVar(self.__obj)

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
        
    def GetWeight(self):
        return self.__lib.TMVATool_GetWeight(self.__obj)
        
    def GetVars(self):
        ptr=self.__lib.TMVATool_GetVars(self.__obj)
        Vars=[ptr[i] for i in range(self.__nvar)]
        return Vars
        
    def GetSpectatorVars(self):
        ptr=self.__lib.TMVATool_GetSpectatorVars(self.__obj)
        Vars=[ptr[i] for i in range(self.__nvar)]
        return Vars
        
    def GetLabel(self):
        ptr=self.__lib.TMVATool_GetLabel(self.__obj)
        cst=cast(ptr,c_char_p).value
        return cst

    def GetEvents(self,sample,index):
        x,y,w=[],[],[]

        while self.NextEvent(sample,index):
            x.append(self.GetVars())
            y.append(self.GetLabel())
            w.append(self.GetWeight())

        return x,y,w
