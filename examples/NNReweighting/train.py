#!/usr/bin/env python

import pymva

import sys,os
from sklearn.utils import shuffle
import numpy,math,array
from ROOT import TGraph, TFile, TTree

argc,argv,argp = pymva.tools.ParseConfig(sys.argv)

#Configuration
tool = pymva.kit.MVAKit('Training')
if argp['ntup_prepared']:
    tool.Parser(argc,argv,0)
    tool.isClassification(0)
else:
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

for i in range(split_size):
    print ('\nReading data from Py ...')
    x_train,y_train,w_train=pymva.tools.ReadFile(ntupleName,'TrainTree'+str(i),tool.Variables)
    x_test,y_test,w_test=pymva.tools.ReadFile(ntupleName,'TestTree'+str(i),tool.Variables)

    print ('  Finish to obtain data ..')
    print ('  Total train: %i, total test: %i'%(len(y_train),len(y_test)))
    
    #arrange datas
    print ('  Transforming data ..')
    x_train_scaled,x_test_scaled=pymva.tools.StdTransform(x_train,x_test,tool.Variables)
    print ('  Shuffling data ..')
    x_train_scaled_shuf,y_train,w_train = shuffle(x_train_scaled,y_train,w_train,random_state=0);
    
    #train data
    model = pymva.ml.GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    pymva.ml.TrainKerasModel("./",model,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train)

    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)
    
    #train data with dropout
    arch_with_dropout = pymva.ml.AddDropout(tool.GetArchitectureOpt(),Dropout=0.2)
    model_dropout = pymva.ml.GetKerasModel(tool.NVar,arch_with_dropout)
    pymva.ml.TrainKerasModel("./",model_dropout,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train)
    ypred_avg_train,yunc_train = pymva.ml.predict_with_uncertainty(x_train_scaled, model_dropout, n_iter=500)
    ypred_avg_test,yunc_test = pymva.ml.predict_with_uncertainty(x_test_scaled, model_dropout, n_iter=500)

    ypred_train = numpy.append(ypred_train,ypred_avg_train,axis=1)
    ypred_train = numpy.append(ypred_train,yunc_train,axis=1)
    ypred_test = numpy.append(ypred_test,ypred_avg_test,axis=1)
    ypred_test = numpy.append(ypred_test,yunc_test,axis=1)

    x_fun = numpy.arange(start=100,stop=1000,step=9)
    x_fun_scaled = pymva.tools.ReadAndStdTransform(x_fun.reshape(-1,1))
    y_fun = model.predict(x_fun_scaled)
    y_fun_m,y_fun_unc = pymva.ml.predict_with_uncertainty(x_fun_scaled, model_dropout, n_iter=500)

    n = int(100)
    x, y, yup = array.array( 'd' ), array.array( 'd' ), array.array( 'd' )

    for j in range(n):
        x.append(x_fun[j])
        y.append(y_fun[j]/(1-y_fun[j]))
        err = (y_fun[j]/(1-y_fun[j]))*math.sqrt(2.0)*(y_fun_unc[j]/y_fun[j])
        yup.append((y_fun[j])/(1-y_fun[j])+err)
        print (('%i %f %f %f')%(j,x[j],y[j],yup[j]))

    g = TGraph(100,x,y)
    gup = TGraph(100,x,yup)
    
    print (ypred_train)
    print (ypred_avg_train)
    print (yunc_train)
    
    #save everything
    model.save('keras_output/model/model_'+str(i)+'.h5')
    model_dropout.save('keras_output/model/model_dropout'+str(i)+'.h5')
    model_dropout.save_weights('keras_output/model/model_dropout_weights'+str(i)+'.h5')
    pymva.tools.UpdateFile(ntupleName,['TrainTree'+str(i),'TestTree'+str(i)],[ypred_train,ypred_test])

    tfile = TFile(ntupleName,"update")
    g.Write('g')
    gup.Write('gup')
    tfile.Write()
    tfile.Close()
