#!/usr/bin/env python

from pymva.tool import TMVATool
from pymva.extra import *
import sys,os
import numpy as np
from scipy.sparse import coo_matrix

from sklearn.utils import shuffle

argv = sys.argv
argc = len(sys.argv)

#configuration
tool = TMVATool('Training')
tool.Parser(argc,argv)
tool.ReadConf()
tool.SetEvents()

#directory to store 
if not os.path.exists('./keras_output'):os.makedirs('./keras_output')
if not os.path.exists('./keras_output/feature_weight'):os.makedirs('./keras_output/feature_weight')
if not os.path.exists('./keras_output/model'):os.makedirs('./keras_output/model')
#loop for n fold
from keras.models import Sequential
for i in range(tool.GetNSplit()):
    #obtain data
    x_train,xspec_train,label_train,w_train=tool.GetEvents('Training',i)
    x_test,xspec_test,label_test,w_test=tool.GetEvents('Testing',i)
    labelTag = Labeling(tool.GetLabelOpt())
    y_train,y_test=GetDigitLabel(label_train,labelTag),GetDigitLabel(label_test,labelTag)
    print ('Total train: %i test: %i'%(len(y_train),len(y_test)))
    
    #arrange data
    x_train,xspec_train,label_train,y_train,w_train = shuffle(x_train,xspec_train,
                                                              label_train,y_train,
                                                              w_train,random_state=0);
    
    #ScaleWeights(y_train,w_train)
    #x_train_scaled,x_test_scaled=PCAStdTransform(x_train,y_train,x_test,tool.Variables)
    x_train_scaled,x_test_scaled=StdTransform(x_train,x_test,tool.Variables)

    #train data
    model = GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    TrainKerasModel(model,tool.GetEngineOpt(),x_train_scaled,y_train,w_train)
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)

    #save everything
    model.save('keras_output/model/model_'+str(i)+'.h5')
    model.save_weights('keras_output/model/weight_'+str(i)+'.h5')
    SaveToNtuple('keras_output/train_'+str(i)+'.root'
                 ,CorrectNames(tool.Variables),CorrectNames(tool.SpecVariables)
                 ,x_train,xspec_train,label_train,y_train,ypred_train,w_train
                 ,x_test,xspec_test,label_test,y_test,ypred_test,w_test
    )
