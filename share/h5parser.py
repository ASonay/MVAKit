#!/usr/bin/env python

# load and evaluate a saved model
import pymva
#from pymva.extra import *
import ROOT as root
import argparse
from sklearn.preprocessing import StandardScaler
from sklearn.externals.joblib import dump,load
import tensorflow as tf

import sys

if __name__ == "__main__":

    parser = argparse.ArgumentParser("NN keras output to txt format", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-p","--path",action="store", dest="path", help="path where the keras model file (h5) saved", required=True)
    parser.add_argument("-c","--conf",action="store", dest="conf", help="Configuration", required=False)
    parser.add_argument("-s","--doScaler",action="store_true", dest="doScaler", help="Decode your scaler too?",default=False, required=False)
    args = parser.parse_args()

    path = args.path
    config = args.conf

    import tensorflow as tf
    model_file=path+'keras_output/model/model_0.h5'
    model_output = path+'keras_output/model/model_0.txt'
    if '.h5' in path:
        model_file = path
        model_output = path.replace('.h5','.txt')
    print ('MODEL : ',model_file)
    model=tf.keras.models.load_model(model_file,
                                     custom_objects=None,
                                     compile=False)
    if args.doScaler:
        tool = pymva.kit.MVAKit('Training')
        tool.isClassification(0)
        tool.isExe(0)
        tool.SetConf(config)
        tool.ReadConf()
    
        scaler = load(path+'keras_output/feature_weight/fold0_std_scaler.bin')
    
        x = [[0,7,132797.15,66186.695,45201.695,37540.648,30212.027,29805.652,25537.996,0,0,0,73784.015,51631.25,1.2]]
        x_s = scaler.transform(x)

        print (x)
        print (x_s)

        print('Normalization:\n')
        with open(path+'keras_output/feature_weight/fold0_std_scaler.txt', 'w') as fout:
            for i,var in enumerate(tool.Variables):
                fout.write(var + ' ' + str(scaler.mean_[i]) + ' ' + str(scaler.scale_[i]) + '\n')
                print ('%-40s | %-6.2e | %-6.2e'%(var,scaler.mean_[i],scaler.scale_[i]))

    ind=0
    with open(model_output, 'w') as fout:
        for layer in model.layers:
            if not isinstance(layer,tf.keras.layers.Dense): continue
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
            ind+=1

    
