#!/usr/bin/env python

import pymva
import sys,os
from sklearn.utils import shuffle
import numpy,math
from ROOT import TGraph, TFile, TTree
import csv

import numpy as np

from tensorflow.keras import backend as K
from tensorflow.python.keras.callbacks import Callback
class Differential_Loss(Callback):
    def __init__(self, x, y, w):
        self.x = x
        self.w = w
        self.y = y
        self.lrate = []
        self.dL = []
        self.dL_av = 0.0
        
    def on_epoch_end(self, epoch, logs={}):
        optimizer = self.model.optimizer
        lr = K.get_value( optimizer.lr(optimizer.iterations) )
        print("\nLearning Rate: " + str(lr))
        if epoch>250:
            y_hat = np.asarray(self.model.predict(self.x))
            len_y = len(self.y)
            dL = 0.0
            for i in range(len_y):
                if (self.y[i] == 1):
                    dL += -0.5*math.exp(-0.5*y_hat[i])*self.w[i]
                else:
                    dL += 0.5*math.exp(0.5*y_hat[i])*self.w[i]

            self.lrate.append(lr)
            self.dL.append(dL)
            self.dL_av += dL/10.

            print("Differential loss: " + str(dL))
            print("Avarage for 10 row: " + str(self.dL_av))

            if (epoch+1)%10==0:
                if abs(self.dL_av) < 0.1 : self.model.stop_training = True
                self.dL_av = 0.0
            

        
argc,argv,argp = pymva.tools.ParseConfig(sys.argv)


#Configuration
tool = pymva.kit.MVAKit('Training')
if argp['ntup_prepared']:
    tool.Parser(argc,argv,0)
    tool.isClassification(0)
else:
    tool.Parser(argc,argv)
tool.ReadConf()

if 'path' in argp:
    path = argp['path']
else:
    path = './'

if path[-1] != '/': path += '/'
    
path += 'pnnrew'+pymva.tools.RemoveSpecialChars(tool.GetEngineOpt())+'/'
if not os.path.exists(path):os.makedirs(path)

#Preparing data into new root file
ntupleName=path+'pyMVAKit.root'
if argp['ntup_prepared']:
    ntupleName=argp['ntup']
else:
    tool.SetFile(ntupleName)
    tool.SetEvents()
    tool.CloseFile()

#directory to store 
if not os.path.exists(path+'keras_output'):os.makedirs(path+'keras_output')
if not os.path.exists(path+'keras_output/feature_weight'):os.makedirs(path+'keras_output/feature_weight')
if not os.path.exists(path+'keras_output/model'):os.makedirs(path+'keras_output/model')

split_size = tool.NSplit if tool.NSplit > 0 else 1 

for i in range(split_size):
    print ('\nReading data from Py ...')
    x_train,y_train,w_train=pymva.tools.ReadFile(ntupleName,'TrainTree'+str(i),tool.Variables)
    x_test,y_test,w_test=pymva.tools.ReadFile(ntupleName,'TestTree'+str(i),tool.Variables)

    print ('  Finish to obtain data ..')
    print ('  Total train: %i, total test: %i'%(len(y_train),len(y_test)))
    
    #arrange datas
    print ('  Transforming data ..')
    x_train_scaled,x_test_scaled=pymva.tools.StdTransform(x_train,x_test,tool.Variables,save_loc=path+'keras_output/feature_weight/fold'+str(i)+'_')
    print ('  Shuffling data ..')
    x_train_scaled_shuf,y_train,w_train = shuffle(x_train_scaled,y_train,w_train,random_state=0);
    #print ('  Scaling weights ..')
    #ScaleWeights(y_train,w_train)

    
    #train data
    model = pymva.ml.GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    #Custom callback
    dL = Differential_Loss(x_train_scaled_shuf,y_train,w_train)
    pymva.ml.TrainKerasModel(path,model,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train,dL)
    model.save(path+'keras_output/model/model_'+str(i)+'.h5')
    pymva.ml.SaveKerasModel(model,path+'keras_output/model/model_0.txt')
    
    #Write learning rate and diff loss
    dL_rate = zip(dL.lrate,dL.dL)
    with open(path+'keras_output/dL_rate.csv', 'w') as f:
        writer = csv.writer(f, delimiter='\t')
        writer.writerows(dL_rate)
        
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)
    
    #train data with dropout
    if argp['unc']:
        arch_with_dropout = pymva.ml.AddDropout(tool.GetArchitectureOpt(),Dropout=0.1)
        model_dropout = pymva.ml.GetKerasModel(tool.NVar,arch_with_dropout)
        pymva.ml.TrainKerasModel(model_dropout,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train)
        model_dropout.save(path+'keras_output/model/model_dropout'+str(i)+'.h5')
        model_dropout.save_weights(path+'keras_output/model/model_dropout_weights'+str(i)+'.h5')
        ypred_avg_train,yunc_train = pymva.ml.predict_with_uncertainty(x_train_scaled, model_dropout, n_iter=1000)
        ypred_avg_test,yunc_test = pymva.ml.predict_with_uncertainty(x_test_scaled, model_dropout, n_iter=1000)

        ypred_train = numpy.append(ypred_train,ypred_avg_train,axis=1)
        ypred_train = numpy.append(ypred_train,yunc_train,axis=1)
        ypred_test = numpy.append(ypred_test,ypred_avg_test,axis=1)
        ypred_test = numpy.append(ypred_test,yunc_test,axis=1)
    
    #Create new ntuple
    ntup_opt = 'recreate' if i == 0 else 'update'
    pymva.tools.CloneFile(path,ntupleName,['TrainTree'+str(i),'TestTree'+str(i)],[ypred_train,ypred_test],'_clone',ntup_opt=ntup_opt)
