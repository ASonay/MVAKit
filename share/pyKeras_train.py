#!/usr/bin/env python

from pymva.kit import MVAKit
from pymva.extra import *
import sys,os
from sklearn.utils import shuffle

argv = sys.argv
argc = len(sys.argv)

#Configuration
tool = MVAKit('Training')
tool.Parser(argc,argv)
tool.ReadConf()

#Preparing data into new root file
ntupleName='pyMVAKit.root'
tool.SetFile(ntupleName)
tool.SetEvents()
tool.CloseFile()

#directory to store 
if not os.path.exists('./keras_output'):os.makedirs('./keras_output')
if not os.path.exists('./keras_output/feature_weight'):os.makedirs('./keras_output/feature_weight')
if not os.path.exists('./keras_output/model'):os.makedirs('./keras_output/model')

split_size = tool.NSplit if tool.NSplit > 0 else 1 

for i in range(tool.NSplit):
    print ('\nReading data from Py ...')
    x_train,y_train,w_train=ReadFile(ntupleName,'TrainTree'+str(i),tool.Variables)
    x_test,y_test,w_test=ReadFile(ntupleName,'TestTree'+str(i),tool.Variables)

    print ('  Finish to obtain data ..')
    print ('  Total train: %i, total test: %i'%(len(y_train),len(y_test)))
    
    #arrange datas
    print ('  Transforming data ..')
    x_train_scaled,x_test_scaled=PCAStdTransform(x_train,y_train,x_test,tool.Variables)
    print ('  Shuffling data ..')
    x_train_scaled_shuf,y_train,w_train = shuffle(x_train_scaled,y_train,w_train,random_state=0);
    print ('  Scaling weights ..')
    ScaleWeights(y_train,w_train)
    
    #train data
    model = GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    TrainKerasModel(model,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train)
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)
    #save everything
    model.save('keras_output/model/model_'+str(i)+'.h5')
    UpdateFile(ntupleName,'TrainTree'+str(i),'TestTree'+str(i),ypred_train,ypred_test)
