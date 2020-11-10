
from TMVAToolWrapper import TMVATool
import sys
import numpy as np

argv = sys.argv
argc = len(sys.argv)

tool = TMVATool('Training')
tool.Parser(argc,argv)
tool.ReadConf()
tool.SetEvents()

x,y,w=tool.GetEvents('Training',0)

print (np.array(x))
print (np.array(y))
print (np.array(w))

print (len(y))
