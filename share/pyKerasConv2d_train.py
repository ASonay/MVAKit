#!/usr/bin/env python

import numpy as np
import pymva
import sys,os,math
from sklearn.utils import shuffle
from ROOT import TLorentzVector

import tensorflow as tf

class PhysicsLayer(tf.keras.layers.Layer):
    def __init__(self, input_dim, nbin=64):
        super(PhysicsLayer, self).__init__()
        self.nbin = nbin

    def call(self, inputs):
        zeros = tf.zeros(shape=(self.nbin, self.nbin, 1))
        return tf.reshape(zeros,(None,)+(self.nbin, self.nbin, 1))
    
    def compute_output_shape(self, input_shape):
        return (input_shape[0],)+(self.nbin, self.nbin, 1)

#def translate_to_physics_layer(x):
    

def get_model():
    cnn_input = tf.keras.layers.Input(shape=(None,))
    print(cnn_input.shape)
    convert_input = PhysicsLayer(cnn_input.shape[0])(cnn_input)
    print(convert_input.shape)
    conv1 = tf.keras.layers.Conv2D(filters = 32, kernel_size=(5, 5), padding = 'Same', activation='relu')(convert_input)
    pool1 = tf.keras.layers.MaxPooling2D(pool_size=(2, 2))(conv1)
    conv2 = tf.keras.layers.Conv2D(filters = 16, kernel_size=(5, 5), padding = 'Same', activation='relu')(pool1)
    pool2 = tf.keras.layers.MaxPooling2D(pool_size=(2, 2))(conv2)

    hidden1 = tf.keras.layers.Dense(10, activation='relu')(pool2)
    output = tf.keras.layers.Dense(1, activation='sigmoid')(hidden1)

    model = tf.keras.models.Model(inputs=cnn_input, outputs=output)

    # summarize layers
    print(model.summary())

    return model

def convert_data(x,nbin=64,xmin=-3.2,xmax=3.2):
    delta = (xmax-xmin)/nbin

    pt_max,tag1_max,m_max,tag2_max = -1e16,-1e16,-1e16,-1e16
    pt_min,tag1_min,m_min,tag2_min = 1e16,1e16,1e16,1e16

    x_train = np.empty((4,len(x['pt'])))

    pip1, pip2, pim= TLorentzVector(), TLorentzVector(), TLorentzVector()
    print(x['pt'][0])
    for eno in range(len(x['pt'])):
        zero_pt = np.empty((nbin,nbin))
        zero_tag1 = np.empty((nbin,nbin),dtype=np.int16)
        zero_m = np.empty((nbin,nbin))
        zero_tag2 = np.empty((nbin,nbin),dtype=np.int16)
        for ono in range(len(x['pt'][eno])):
            eta = x['eta'][eno][ono]
            phi = x['phi'][eno][ono]
            pt = x['pt'][eno][ono]
            e = x['e'][eno][ono]
            tag = x['tag'][eno][ono]
            i = int((eta - xmin)/delta)
            j = int((phi - xmin)/delta)
            zero_pt[i][j] += float(pt)/1.0e06
            zero_tag1[i][j] += int(tag)
            pip1.SetPtEtaPhiE(pt,eta,phi,e)
            for ono_j in range(len(x['pt'][eno])):
                if ono == ono_j: continue
                eta2 = x['eta'][eno][ono_j]
                phi2 = x['phi'][eno][ono_j]
                pt2 = x['pt'][eno][ono_j]
                e2 = x['e'][eno][ono_j]
                tag2 = x['tag'][eno][ono_j]
                deta = abs(eta2-eta)
                dphi = abs(phi2-phi)
                ii = int((deta - 0)/delta)
                jj = int((dphi - 0)/delta)
                pip2.SetPtEtaPhiE(pt2,eta2,phi2,e2)
                pim = pip1+pip2
                zero_m[ii][jj] += pim.M()/1.0e06
                zero_tag2[ii][jj] += int(tag+tag2)
        #mat_pt.append(zero_pt.astype(np.float16))
        #mat_tag1.append(zero_tag1.astype(np.int16))
        #mat_m.append(zero_m.astype(np.float16))
        #mat_tag2.append(zero_tag2.astype(np.int16))

    print('Finished')
    print(zero_m)

    return [mat_pt,mat_tag1,mat_m,mat_tag2]
            

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
    if path[-1] != '/': path += '/'
else:
    path = '.'
    path += '/pyKeras'+pymva.tools.RemoveSpecialChars(tool.GetEngineOpt())+'/'

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

model = get_model()

X = [[1]]
y = [1]

model.compile(loss="binary_crossentropy",optimizer="ADAM")
model.fit(X,y)

"""
for i in range(tool.NSplit):
    print ('\nReading data from Py ...')
    x_train,y_train,w_train=pymva.tools.ReadFile(ntupleName,'TrainTree'+str(i),tool.Variables)
    x_test,y_test,w_test=pymva.tools.ReadFile(ntupleName,'TestTree'+str(i),tool.Variables)
    x_train_conv = convert_data(x_train)
    del x_train
    break

    print ('  Finish to obtain data ..')
    print ('  Total train: %i, total test: %i'%(len(y_train),len(y_test)))
    
    #arrange datas
    print ('  Transforming data ..')
    x_train_scaled,x_test_scaled=pymva.tools.StdTransform(x_train,x_test,tool.Variables,save_loc=path+'keras_output/feature_weight/fold'+str(i)+'_')
    print ('  Shuffling data ..')
    x_train_scaled_shuf,y_train,w_train = shuffle(x_train_scaled,y_train,w_train,random_state=0);
    print ('  Scaling weights ..')
    pymva.tools.ScaleWeights(y_train,w_train)
    
    #train data
    model = pymva.ml.GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    pymva.ml.TrainKerasModel(path,model,tool.GetEngineOpt(),x_train_scaled_shuf,y_train,w_train)
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)
    #save everything
    model.save(path+'keras_output/model/model_'+str(i)+'.h5')
    pymva.ml.SaveKerasModel(model,path+'keras_output/model/model_'+str(i)+'.txt')
    ntup_opt = 'recreate' if i == 0 else 'update'
    pymva.tools.CloneFile(path,ntupleName,['TrainTree'+str(i),'TestTree'+str(i)],[ypred_train,ypred_test],'_clone',ntup_opt=ntup_opt)
"""
