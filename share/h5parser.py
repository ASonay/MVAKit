#!/usr/bin/env python

# load and evaluate a saved model
from pymva.kit import MVAKit
from pymva.extra import *
import ROOT as root
import argparse
from sklearn.preprocessing import StandardScaler
from sklearn.externals.joblib import dump,load

import sys

if __name__ == "__main__":

    parser = argparse.ArgumentParser("NN keras output to txt format", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-p","--path",action="store", dest="path", help="path where the keras model file (h5) saved", required=True)
    parser.add_argument("-c","--conf",action="store", dest="conf", help="Configuration", required=True)
  
    args = parser.parse_args()

    path = args.path
    config = args.conf

    tool = MVAKit('Training')
    tool.isClassification(0)
    tool.isExe(0)
    tool.SetConf(config)
    tool.ReadConf()

    from keras.models import load_model
    model_file=path+'keras_output/model/model_0.h5'
    print ('MODEL : ',model_file)
    model=load_model(model_file,
                     custom_objects=None,
                     compile=False)

    scaler = load(path+'keras_output/feature_weight/fold0_std_scaler.bin')
    
    x = [[0,7,132797.15,66186.695,45201.695,37540.648,30212.027,29805.652,25537.996,0,0,0,73784.015,51631.25]]
    x_s = scaler.transform(x)

    print (x)
    print (x_s)

    print('Normalization:\n')
    with open(path+'keras_output/feature_weight/fold0_std_scaler.txt', 'w') as fout:
        for i,var in enumerate(tool.Variables):
            fout.write(var + ' ' + str(scaler.mean_[i]) + ' ' + str(scaler.scale_[i]) + '\n')
            print ('%-40s | %-6.2e | %-6.2e'%(var,scaler.mean_[i],scaler.scale_[i]))

    with open(path+'keras_output/model/model_0.txt', 'w') as fout:
        for ind,layer in enumerate(model.layers):
            fout.write('Start for layer ' + str(ind) + '\n')
            W = layer.get_weights()[0]
            for i in range(W.shape[0]):
                fout.write('\n')
                for j in range(W.shape[1]):
                    fout.write(str(W[i,j]) + ' ')
            fout.write('\n')
            fout.write('Bias\n')
            bias = layer.get_weights()[1]
            for i in range(bias.shape[0]):
                fout.write(str(bias[i]) + ' ')
            fout.write('\n\n')
            fout.write('End of layer ' + str(ind) + '\n')
            fout.write('\n\n')

    
