#!/usr/bin/env python

# load and evaluate a saved model
from pymva.kit import MVAKit
from pymva.extra import *
import ROOT as root
import argparse

import sys


def evaluate(ntuple_name,tree_name,variable_names):
    print ('NTUPLE: ',ntuple_name)
    print ('TREE  : ',tree_name)

    f_read=root.TFile(ntuple_name)
    tree = f_read.Get(tree_name)
    
    tree_f=[]
    print ('Formulas are appending for :')
    for i in range(len(variable_names)):
        print (variable_names[i])
        tree_f.append(root.TTreeFormula('form_'+str(i),variable_names[i],tree))
        tree_f[-1].SetQuickLoad(True)

    n_entries = tree.GetEntries()
    sample_size = len(tree_f)

    X=[]
    step=int(n_entries/20)

    print('Reading ntuple..')
    for i in range(n_entries):
        tree.GetEntry(i)
        x=[]
        for j,formula in enumerate(tree_f):
            formula.GetNdata()
            x.append(formula.EvalInstance(0))
        X.append(x)
        if (i%step)==0:
            print (i,'/',n_entries,'events has been done...')
    print('Finished..\n')
    f_read.Close()

    return X

def GetTreeNames(ntuple,tree_names):
    f0 = root.TFile(ntuple)

    trees=[]
    keys = f0.GetListOfKeys()
    if tree_names[0] == 'ALL':
        tree_name_pre = 'null_tree_name'
        for key in keys:
            if key.GetClassName() != 'TTree' : continue
            tree_tmp = key.ReadObj()
            tree_name = tree_tmp.GetName()
            noEx = True
            for name in tree_names:
                if name == tree_name: noEx=False
            if tree_name != tree_name_pre and noEx:
                trees.append(tree_name)
                print ('TREE NAME WILL BE FILLED! :  ',tree_name)
            tree_name_pre = tree_name
    else :
        trees=tree_names

    f0.Close()

    return trees

    
if __name__ == "__main__":

    parser = argparse.ArgumentParser("NN evaluate tool", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-n","--ntup", nargs='*', action="store", dest="ntuples", help="Ntuple file to be evaluated.", required=True)
    parser.add_argument("-t","--trees",action="store", dest="trees", help="ROOT Trees to be evaluated", required=True)
    parser.add_argument("-p","--path",action="store", dest="path", help="path where the keras model file (h5) saved", required=True)
    parser.add_argument("-c","--conf",action="store", dest="conf", help="Configuration", required=True)
    parser.add_argument("-pf","--prefix",action="store", dest="prefix", help="Prefix to be added variable name (score+prefix)",default="prefx", required=False)
    parser.add_argument("-pf_ntup","--prefix_ntup",action="store", dest="prefix_ntup", help="Prefix to be added end of ntuple",default="", required=False)
    parser.add_argument("-u","--update",action="store_true", dest="update", help="Only update the desired ntuple", required=False)
   
    args = parser.parse_args()

    ntuples = args.ntuples
    tree_names = args.trees.split(',')
    path = args.path
    prefix = args.prefix
    config = args.conf

    tool = MVAKit('Training')
    tool.isClassification(0)
    tool.isExe(0)
    tool.SetConf(config)
    tool.ReadConf()
    
    import tensorflow as tf 
    model_file=path+'keras_output/model/model_0.h5'
    print ('MODEL : ',model_file)
    model=tf.keras.models.load_model(model_file,
                     custom_objects=None,
                     compile=False)

    for ntuple_name in ntuples:
        trees=GetTreeNames(ntuple_name,tree_names)
        for i,tree_name in enumerate(trees):
            x = evaluate(ntuple_name,tree_name,tool.VariablesOriginal)
            x_scaled = ReadAndStdTransform(x,save_loc=path+'keras_output/feature_weight/fold0_')
            y = model.predict(x_scaled)
            ntup_opt = 'recreate' if i == 0 else 'update'
            score_name = 'score'+prefix
            if args.update:
                print ('Updating file...')
                UpdateFile(ntuple_name,[tree_name],[y],score_name)
            else:
                print('Cloning file...')
                CloneFile(path,ntuple_name,[tree_name],[y],args.prefix_ntup,score_name,ntup_opt,True)
