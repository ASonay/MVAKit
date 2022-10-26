import numpy as np
import pandas as pd
from ROOT import TFile, TTree, vector, TCut, std
from array import array
import uproot

def GetDigitLabel(label,dic={'sig':1.0,'bkg':0.0}):
    y=[]
    for l in label:
        y.append(dic[l])
    return y

def Labeling(string):
    label_tags_dic = {}
    full_str = string.split(',')
    if full_str:
        for tag in full_str:
            split_tag = tag.split(';')
            if len(split_tag) == 2:
                split_lab = split_tag[1].split('|')
                if len(split_lab) > 1:
                    lab_list=[]
                    for lab in split_lab: lab_list.append(float(lab))
                    label_tags_dic[split_tag[0]]=lab_list
                else:
                    label_tags_dic[split_tag[0]]=float(split_tag[1])
            else:
                print ('Your labeling format is false, please try following.')
                print ('LABEL1;VAL1_1|..|VAL1_2,..,LABELN;VALN_1|...|VALN_M')
                exit()
    else:
        print ('No labeling option found!')

    return label_tags_dic

def ReadFile(fil,tr,variables):
    flat_tree = uproot.open(fil)[tr]
    df = flat_tree.pandas.df()

    x=pd.concat([df.pop(x) for x in variables], axis=1)
    y=df.pop('classID').astype(np.int8)
    w=df.pop('weight').astype(np.float16)

    del(df,flat_tree)
    
    return x.values,y.values,w.values

def UpdateFile(fil,tree_names,y_pred,var_name='score'):
    print (('FileName to be read: %s')%fil)
    tfile = TFile(fil,'update')
    trees=[]
    
    if len(tree_names) != len(y_pred):
        print('Number of trees and number of prediction must be equal')
        exit()
        
    for t in tree_names:
        print ('Tree will be updated: %s'%t)
        trees.append(tfile.Get(t))

    score = vector('float')()
    print ('\nUpdating File --------------------------')
    for t in trees:
        t.Branch(var_name,score)
    
    for i in range(len(trees)):
        for x in y_pred[i]:
            score.clear()
            if not np.isscalar(x):
                for e in x: score.push_back(e)
            else:
                score.push_back(x)
            trees[i].GetBranch(var_name).Fill()
        trees[i].Write()
    tfile.Close()
    del score
    del trees[:]
    del tfile
    print ('Closing File --------------------------\n')

def CloneFile(path,fil,tree_names,y_pred,pref='',var_name='score',ntup_opt='recreate',same_path=False):
    print (('FileName to be read: %s')%fil)
    tfile = TFile(fil)
    trees=[]
    
    if len(tree_names) != len(y_pred):
        print('Number of trees and number of prediction must be equal')
        exit()
        
    for t in tree_names:
        print ('Tree will be cloned: %s'%t)
        trees.append(tfile.Get(t))

    score = vector('float')()
    print ('\nUpdating File --------------------------')
    if (ntup_opt=='recreate' and pref==''):
        print('Attention :: you are trying to create a new ntuple with same file name.')
        print('Please specify a prefix.')
        exit()
    fil_new = fil.replace('.root',pref+'.root')
    if not same_path:
        fil_new = path+fil[fil.rfind('/')+1:]
    print (('FileName to be recorded: %s')%fil_new)
    trees_new=[]
    tfile_new = TFile(fil_new,ntup_opt)
    for t in trees:
        trees_new.append(t.CloneTree())
        trees_new[-1].Branch(var_name,score)
    
    for i in range(len(trees_new)):
        for x in y_pred[i]:
            score.clear()
            if not np.isscalar(x):
                for e in x: score.push_back(e)
            else:
                score.push_back(x)
            trees_new[i].GetBranch(var_name).Fill()
        trees_new[i].Write()
    tfile_new.Write()
    tfile_new.Close()
    tfile.Close()
    del score
    del trees[:]
    del trees_new[:]
    del tfile
    del tfile_new
    print ('Closing File --------------------------\n')
    return fil_new

#To-Do: make it also adaptable for multiclass
def ScaleWeights(y,w):
    sum_wpos = sum( w[i] for i in range(len(y)) if y[i] == 1.0  )
    sum_wneg = sum( w[i] for i in range(len(y)) if y[i] == 0.0  )

    for i in range(len(w)):
        if (y[i]==1.0):
            w[i] = w[i] * (0.5/sum_wpos)
        else:
            w[i] = w[i] * (0.5/sum_wneg)

    w_av = sum(w)/len(w)
    w[:] = [x/w_av for x in w]
    
    sum_wpos_check = sum( w[i] for i in range(len(y)) if y[i] == 1.0  )
    sum_wneg_check = sum( w[i] for i in range(len(y)) if y[i] == 0.0  )

    print ('\n======Weight Statistic========================================')
    print ('Weights::        W(1)=%g, W(0)=%g' % (sum_wpos, sum_wneg))
    print ('Scaled weights:: W(1)=%g, W(0)=%g' % (sum_wpos_check, sum_wneg_check))
    print ('==============================================================')

from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.naive_bayes import GaussianNB
from sklearn.pipeline import make_pipeline
from joblib import dump,load

from sklearn import metrics
def PCAStdTransform(x_train,y_train,x_test,varnames=[],save_loc='keras_output/feature_weight/'):
    ncomp = len(x_train[0])
    #-------------------------------------------------------------------
    std_clf = make_pipeline(StandardScaler(), PCA(n_components=ncomp), GaussianNB())
    std_clf.fit(x_train, y_train)
    pred_test_std = std_clf.predict(x_test)
    pca_std = std_clf.named_steps['pca']
    scaler = std_clf.named_steps['standardscaler']
    x_train_scaled = pca_std.transform(scaler.transform(x_train))
    x_test_scaled = pca_std.transform(scaler.transform(x_test))
    #-------------------------------------------------------------------
    print ('\n======PCA Transformation=====================================')
    print ('%-40s'%('Variables')),
    for i in range(ncomp):
        varname = 'Var_'+str(i)
        if varnames[i]: varname = varnames[i]
        if i!=ncomp-1:
            print (' | %s'%(varname)),
        else:
            print (' | %s'%(varname))
    for i in range(ncomp):
        varname = 'Var_'+str(i)
        if varnames[i]: varname = varnames[i]
        print ('%-40s'%(varname)),
        for j,x in enumerate(pca_std.components_[i]):
            if j!=ncomp-1:
                print ('| %-8.2e'%(x)),
            else:
                print ('| %-8.2e'%(x))
    print ('==============================================================')
    print ('\n======STD Transformation=====================================')
    print ('%-40s | %-8s | %-8s | %-8s | %-8s'%('Variables','min','max','mean','variance'))
    for i in range(ncomp):
        column=[x[i] for x in x_train]
        min_c=min(column) 
        max_c=max(column)
        varname = 'Var_'+str(i)
        if varnames[i]: varname = varnames[i]
        print ('%-40s | %-6.2e | %-6.2e | %-6.2e | %-6.2e'%(varname,min_c,max_c,scaler.mean_[i],scaler.var_[i]))
    print ('==============================================================')
    #-------------------------------------------------------------------    
    dump(scaler, save_loc+'std_scaler.bin', compress=True)
    dump(pca_std, save_loc+'pca.bin', compress=True)
    return x_train_scaled,x_test_scaled

def StdTransform(x_train,x_test,varnames=[],save_loc='keras_output/feature_weight/'):
    ncomp = len(x_train[0])
    #-------------------------------------------------------------------
    scaler = StandardScaler()
    scaler.fit(x_train)
    x_train_scaled = scaler.transform(x_train)
    x_test_scaled = scaler.transform(x_test)
    #-------------------------------------------------------------------
    print ('\n======STD Transformation=====================================')
    print ('%-40s | %-8s | %-8s | %-8s | %-8s'%('Variables','min','max','mean','variance'))
    with open(save_loc+'std_scaler.txt', 'w') as fout:
        for i in range(ncomp):
            column=[x[i] for x in x_train]
            min_c=min(column) 
            max_c=max(column)
            varname = 'Var_'+str(i)
            if varnames[i]: varname = varnames[i]
            fout.write(varname + ' ' + str(scaler.mean_[i]) + ' ' + str(scaler.scale_[i]) + '\n')
            print ('%-40s | %-6.2e | %-6.2e | %-6.2e | %-6.2e'%(varname,min_c,max_c,scaler.mean_[i],scaler.var_[i]))
    print ('==============================================================')
    #-------------------------------------------------------------------    
    dump(scaler, save_loc+'std_scaler.bin', compress=True)
    return x_train_scaled,x_test_scaled

def ReadAndStdTransform(x,save_loc='keras_output/feature_weight/'):
    scaler = load(save_loc+'std_scaler.bin')
    return scaler.transform(x)


def CorrectNames(var):
    for i in range(len(var)):
        var[i]=var[i].replace('(','_')
        var[i]=var[i].replace(')','_')
        var[i]=var[i].replace('&','')
        var[i]=var[i].replace('$','')
        var[i]=var[i].replace('[','')
        var[i]=var[i].replace(']','')
        var[i]=var[i].replace('-','')
        var[i]=var[i].replace('+','')
        var[i]=var[i].replace('<','')
        var[i]=var[i].replace('/','_')
        var[i]=var[i].replace('>','')
        var[i]=var[i].replace('*','')
        var[i]=var[i].replace(',','_')
        var[i]=var[i].replace('=','_')

    return var

def RemoveSpecialChars(string):
    chars = [':','.','(',')','"','=','/',';',' ','|',',']
    replacewith = '_'
    print (string)
    for c in chars:
        string = string.replace(c,replacewith)
    return string

def ParseConfig(argv):
    dic = {}
    argv_new = []
    dic['ntup_prepared'] = False
    dic['unc'] = False
    dic['prefix'] = 'pref'
    dic['tree'] = 'nominal_Loose'
    i=0
    while i<len(argv):
        if argv[i] == '--prepared-ntup' or argv[i] == '--ntup-prepared' or argv[i] == '--clone-ntup':
            dic['ntup_prepared'] = True
            dic['ntup'] = argv[i+1]
            i+=2
        elif argv[i] == '--uncertainty':
            dic['unc'] = True
            i+=1
        elif argv[i] == '--path':
            dic['path'] = argv[i+1]
            i+=2
        elif argv[i] == '--tree':
            dic['tree'] = argv[i+1]
            i+=2
        elif argv[i] == '--prefix':
            dic['prefix'] = argv[i+1]
            i+=2
        else:
            argv_new.append(argv[i])
            i+=1

    if 'path' in dic:
        if dic['path'][-1] == '/':
            dic['path'] = dic['path'][:-1]
        
    argv_n = len(argv_new)

    return argv_n,argv_new,dic
