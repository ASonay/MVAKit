#!/usr/bin/env python

from pymva.kit import MVAKit
from pymva.extra import *
import sys,os
from keras.models import Sequential
import numpy as np
from scipy.sparse import coo_matrix
from sklearn.utils import shuffle

argv = sys.argv
argc = len(sys.argv)

#configuration
tool = MVAKit('Training')
tool.Parser(argc,argv)
tool.ReadConf()
tool.SetFile('PreData.root')
tool.SetEvents()
tool.CloseFile()


#directory to store 
if not os.path.exists('./keras_output'):os.makedirs('./keras_output')
if not os.path.exists('./keras_output/feature_weight'):os.makedirs('./keras_output/feature_weight')
if not os.path.exists('./keras_output/model'):os.makedirs('./keras_output/model')

for i in range(tool.NSplit):
    print ('\nPre-processing the data ...')
    x_train,xspec_train,y_train,w_train=ReadFile('PreData.root','TrainTree',i)
    x_test,xspec_test,y_test,w_test=ReadFile('PreData.root','TestTree',i)
    print ('  Finish to obtain data ..')
    print ('  Total train: %i test: %i'%(len(y_train),len(y_test)))
    
    #arrange data
    print ('  Shuffling data ..')
    x_train,xspec_train,y_train,w_train = shuffle(x_train,xspec_train,y_train,
                                                  w_train,random_state=0);
    print ('  Transforming data ..')
    x_train_scaled,x_test_scaled=PCAStdTransform(x_train,y_train,x_test,tool.Variables)

    print ('  Scaling weights ..')
    ScaleWeights(y_train,w_train)

    #train data
    model = GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    TrainKerasModel(model,tool.GetEngineOpt(),x_train_scaled,y_train,w_train)
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)
    #save everything
    model.save('keras_output/model/model_'+str(i)+'.h5')
    SaveToNtuple('keras_output/train_'+str(i)+'.root'
                 ,CorrectNames(tool.Variables),CorrectNames(tool.SpecVariables)
                 ,x_train,xspec_train,y_train,ypred_train,w_train
                 ,x_test,xspec_test,y_test,ypred_test,w_test
    )
