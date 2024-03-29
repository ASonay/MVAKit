from ctypes import *
import os

class MVAKit(object):
    def __init__(self,name='Training'):
        cpp_lib = os.environ['MVAKIT_HOME']+'/build/lib/libMVAKit.so'
        if (os.path.exists(cpp_lib)):
            print('Succesfully found your C++ library:',cpp_lib)
        else:
            print('Cannot find your C++ library. Please check if MVAKit has been built')
            exit()
        self.__lib = cdll.LoadLibrary(cpp_lib)
        self.NSplit = 0
        self.__nlabel = 0
        self.NVar = 0
        self.NVarSpec = 0
        self.LabelNames = []
        self.Variables = []
        self.VariablesOriginal = []
        self.SpecVariables = []
        self.__obj = self.__lib.MVAKit_new(c_char_p(name.encode('utf-8')))

    def ReadConf(self):
        self.__lib.MVAKit_ReadConf(self.__obj)
        self.NVar = self.GetNVar()
        self.NVarSpec = self.GetNSpectatorVar()
        self.NSplit = self.GetNSplit()
        self.__nlabel = self.GetNLabel()
        for i in range(self.__nlabel):self.LabelNames.append(self.GetLabelName(i))
        for i in range(self.NVar):self.VariablesOriginal.append(self.GetVariableName(i))
        for i in range(self.NVar):self.Variables.append(self.GetVariableName(i,1))
        for i in range(self.NVarSpec):self.SpecVariables.append(self.GetSpectatorVariableName(i))
        varParam = self.GetParamName()
        if varParam :
            self.Variables.append(varParam)
            self.NVar += 1

    def CloseFile(self):
        self.__lib.MVAKit_CloseFile(self.__obj)
        
    def SetFile(self,name):
        self.__lib.MVAKit_SetFile(self.__obj,c_char_p(name.encode('utf-8')))

    def CloseCSV(self):
        self.__lib.MVAKit_CloseCSV(self.__obj)
        
    def SetCSV(self,name):
        self.__lib.MVAKit_SetCSV(self.__obj,c_char_p(name.encode('utf-8')))
        
    def SetConf(self,name):
        self.__lib.MVAKit_SetConf(self.__obj,c_char_p(name.encode('utf-8')))

    def isClassification(self,i):
        self.__lib.MVAKit_isClassification(self.__obj,c_int(i))
        
    def isExe(self,i):
        self.__lib.MVAKit_isExe(self.__obj,c_int(i))

    def GetNVar(self):
        return self.__lib.MVAKit_GetNVar(self.__obj)

    def GetNSplit(self):
        return self.__lib.MVAKit_GetNSplit(self.__obj)

    def GetNLabel(self):
        return self.__lib.MVAKit_GetNLabel(self.__obj)

    def GetNSpectatorVar(self):
        return self.__lib.MVAKit_GetNSpectatorVar(self.__obj)
        
    def Parser(self,argc,argv,reqntup=1):
        arg_p = (c_char_p * argc)()
        argv_bit = []
        for arg in argv : argv_bit.append(arg.encode('utf-8'))
        arg_p[:] = argv_bit
        arg_pp = POINTER(c_char_p)(arg_p)
        self.__lib.MVAKit_Parser(self.__obj,c_int(argc),arg_pp,c_int(reqntup))

    def SetEvents(self,fileName="",labelName="",doCut=1):
        self.__lib.MVAKit_SetEvents(self.__obj,c_char_p(fileName.encode('utf-8')),c_char_p(labelName.encode('utf-8')),c_int(doCut))

    def GetArchitectureOpt(self):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetArchitectureOpt(self.__obj,s)
        val=s.value
        print ('Architecture Opt:: %s'%(val))
        return val.decode('utf-8')
        
    def GetEngineOpt(self):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetEngineOpt(self.__obj,s)
        val=s.value
        print ('Engine Opt:: %s'%(val))
        return val.decode('utf-8')
        
    def GetLabelOpt(self):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetLabelOpt(self.__obj,s)
        val=s.value
        print ('Label Opt:: %s'%(val))
        return val.decode('utf-8')
        
    def GetLabelName(self,index):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetLabelName(self.__obj,c_int(index),s)
        val=s.value
        print ('Label %i:: %s'%(index,val))
        return val.decode('utf-8')
        
    def GetVariableName(self,index,pair=0):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetVariableName(self.__obj,c_int(index),s,c_int(pair))
        val=s.value
        return val.decode('utf-8')
        
    def GetSpectatorVariableName(self,index,pair=0):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetSpectatorVariableName(self.__obj,c_int(index),s,c_int(pair))
        val=s.value
        return val.decode('utf-8')
        
    def GetParamName(self):
        s=create_string_buffer(b'\000' * 1024)
        self.__lib.MVAKit_GetParamName(self.__obj,s)
        val=s.value
        return val.decode('utf-8')

