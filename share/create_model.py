#!/usr/bin/env python

from pymva.kit import MVAKit
from pymva.extra import *
import sys,os

from keras.models import Sequential

if len(sys.argv)!=4:
    print ('Require 3 parameter:')
    print ('<number of varialbe> <architecture> <hyperparameters>')
    exit()

NVar = int(sys.argv[1])
ArcOpt = sys.argv[2]
EngOpt = sys.argv[3]

#train data
model = GetKerasModel(NVar,ArcOpt)
CompileKerasModel(model,EngOpt)
model.save('model.h5')
