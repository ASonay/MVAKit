
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

from keras.models import Sequential
from keras.layers import Dense, Dropout
def GetKerasModel(input_dim,opt):
    layers = opt.split(',')
    if len(layers) < 2:
        print ('You should have at least two layer for NN')
        exit()
        
    model = Sequential()
    for i,l in enumerate(layers):
        split_layer = l.split(';')
        layer = split_layer[0].split('|')
        if i == 0:
            if len(layer) == 3:
                model.add(Dense(int(layer[1]),
                                input_dim=input_dim,
                                activation=layer[0],
                                kernel_initializer=layer[2]))
            elif len(layer) == 2:
                model.add(Dense(int(layer[1]),
                                input_dim=input_dim,
                                activation=layer[0]))
            else:
                print ('Your architecture format is false, please try following.')
                print ('Activation|node|initializer;dropout,...,Activation|node|initializer;dropout')
                exit()
        else:
            if len(layer) == 3:
                model.add(Dense(int(layer[1]),
                                activation=layer[0],
                                kernel_initializer=layer[2]))
            elif len(layer) == 2:
                model.add(Dense(int(layer[1]),
                                activation=layer[0]))
            else:
                print ('Your architecture format is false, please try following.')
                print ('Activation|node|initializer;dropout,...,Activation|node|initializer;dropout')
                exit()
        if len(split_layer)==2:
            model.add(Dropout(float(split_layer[1])))

    return model

import numpy as np
from keras.optimizers import SGD,Adam,RMSprop
def TrainKerasModel(model,opt,x,y,w):
    getopt = opt.split(',')
    optlist={}
    for o in getopt:
        osplit=o.split(';')
        if len(osplit) == 2:
            optlist[osplit[0].upper()]=osplit[1]
        else:
            print ('Your options has to be like following,')
            print ('opt1;val1,...,optn;valn')
            exit()
    
    lrate=float(optlist['LRATE'])
    momentum=float(optlist['MOMENTUM'])
    nepoch=int(optlist['EPOCH'])
    batch=int(optlist['BATCH'])
    optimizer=optlist['OPTIMIZER']
    loss=optlist['LOSS']

    optim=None
    if optimizer.upper() == 'SGD':
        optim=SGD(lr=lrate, momentum=momentum)
    elif optimizer.upper() == 'ADAM':
        optim=Adam(lr=lrate)
    elif optimizer.upper() == 'RMSPROP':
        optim=RMSprop(lr=lrate, momentum=momentum)
    else:
        print ('Available optimizers: SGD,ADAM,RMSPROP')
        print ('You choose none, SGD will be run..')
        optim=SGD(lr=lrate, momentum=momentum)
        
    model.compile(loss=loss, optimizer=optim, metrics=['accuracy'])
    print (model.summary())
    model.fit(np.array(x), np.array(y), sample_weight = np.array(w), epochs=nepoch, batch_size=batch)

from ROOT import TFile, TTree, vector
from array import array
def SaveToNtuple(fil,var,varSpec,x1,x1spec,l1,y1,y1_pred,w1,x2,x2spec,l2,y2,y2_pred,w2):

    noo=1
    if isinstance(y1[0],list):
        noo=np.array(y1).shape[1]
    print('\nNumber of Outputs:',noo)

    variable_size = len(var)
    variableSpec_size = len(varSpec)

    label = vector('string')()
    class_id = array('i', [ 0 ]*noo )
    variables = variable_size*[0]
    variablesSpec = variableSpec_size*[0]
    for i in range(variable_size):
        variables[i] = array('f', [ 0. ] )
    for i in range(variableSpec_size):
        variablesSpec[i] = array('f', [ 0. ] )
    weight = array('f', [ 0. ] )
    score = array('f', [ 0. ]*noo )

    f = TFile(fil,'recreate')
    t1 = TTree('tr_train','Tree for Training Output')
    t2 = TTree('tr_test','Tree for Test Output')

    t1.Branch('label',label)
    t1.Branch('class_id',class_id,'class_id['+str(noo)+']/I')
    for i in range(variable_size):
        t1.Branch(var[i],variables[i],var[i]+'/F')
    for i in range(variableSpec_size):
        t1.Branch(varSpec[i],variablesSpec[i],varSpec[i]+'/F')
    t1.Branch('weight',weight,'weight/F')
    t1.Branch('score',score,'score['+str(noo)+']/F')

    t2.Branch('label',label)
    t2.Branch('class_id',class_id,'class_id['+str(noo)+']/I')
    for i in range(variable_size):
        t2.Branch(var[i],variables[i],var[i]+'/F')
    for i in range(variableSpec_size):
        t2.Branch(varSpec[i],variablesSpec[i],varSpec[i]+'/F')
    t2.Branch('weight',weight,'weight/F')
    t2.Branch('score',score,'score['+str(noo)+']/F')

    for i in range(len(x1)):
        label.clear()
        label.push_back(l1[i])
        if noo > 1:
            for j in range(noo):
                class_id[j] = int(y1[i][j])
        else:
            class_id[0] = int(y1[i])
        for j in range(variable_size):
            variables[j][0] = x1[i][j]
        for j in range(variableSpec_size):
            variablesSpec[j][0] = x1spec[i][j]
        weight[0] = w1[i]
        for j in range(noo):
            score[j] = y1_pred[i][j]
        t1.Fill()
    t1.Write()

    for i in range(len(x2)):
        label.clear()
        label.push_back(l2[i])
        if noo > 1:
            for j in range(noo):
                class_id[j] = int(y2[i][j])
        else:
            class_id[0] = int(y2[i])
        for j in range(variable_size):
            variables[j][0] = x2[i][j]
        for j in range(variableSpec_size):
            variablesSpec[j][0] = x2spec[i][j]
        weight[0] = w2[i]
        for j in range(noo):
            score[j] = y2_pred[i][j]
        t2.Fill()
    t2.Write()

    f.Write()
    f.Close()
    print ("Storing the data has been done..------------------------")

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
from sklearn.externals.joblib import dump
from sklearn import metrics
def PCAStdTransform(x_train,y_train,x_test,y_test,varnames=[],save_loc='keras_output/feature_weight/'):
    ncomp = len(x_train[0])
    #-------------------------------------------------------------------
    std_clf = make_pipeline(StandardScaler(), PCA(n_components=ncomp), GaussianNB())
    std_clf.fit(x_train, y_train)
    pred_test_std = std_clf.predict(x_test)
    print('\nPrediction accuracy for the standardized test dataset with PCA')
    print('{:.2%}\n'.format(metrics.accuracy_score(y_test, pred_test_std)))
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
    dump(scaler, save_loc+'std_scaler_'+str(i)+'.bin', compress=True)
    dump(pca_std, save_loc+'pca_'+str(i)+'.bin', compress=True)
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
    for i in range(ncomp):
        column=[x[i] for x in x_train]
        min_c=min(column) 
        max_c=max(column)
        varname = 'Var_'+str(i)
        if varnames[i]: varname = varnames[i]
        print ('%-40s | %-6.2e | %-6.2e | %-6.2e | %-6.2e'%(varname,min_c,max_c,scaler.mean_[i],scaler.var_[i]))
    print ('==============================================================')
    #-------------------------------------------------------------------    
    dump(scaler, save_loc+'std_scaler_'+str(i)+'.bin', compress=True)
    return x_train_scaled,x_test_scaled



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
