import numpy as np
from ROOT import TFile, TTree, vector, TCut, std
from array import array

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

def GetKerasModel(input_dim,opt):
    from keras.models import Sequential
    from keras.layers import Dense, Dropout
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

def TrainKerasModel(model,opt,x,y,w):
    from keras.optimizers import SGD,Adam,RMSprop
    from keras.models import Sequential
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
    
def CompileKerasModel(model,opt):
    from keras.optimizers import SGD,Adam,RMSprop
    from keras.models import Sequential
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

def ReadFile(fil,tr,variables):
    data = TFile.Open(fil)
    tree = data.Get(tr)
    label = std.vector('int')(1)
    tree.SetBranchAddress('classID', label)
    
    x,y,w=[],[],[]
    for i in xrange(tree.GetEntries()):
        tree.GetEntry(i)
        x_tmp=[]
        for var in variables:
            x_tmp.append(getattr(tree,var))
        x.append(x_tmp)
        y_tmp=[]
        for j in range(label.size()):
            y_tmp.append(label[j])
        y.append(y_tmp)
        w.append(getattr(tree,'weight'))
    data.Close()
    
    if len(y[0])==1 : y = [e[0] for e in y]
    
    return x,y,w

def UpdateFile(fil,tree_names,y_pred,var_name='score'):
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
    for t in trees:
        trees.append(t.CloneTree())
        trees[-1].Branch(var_name,score)
    
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
    print ('Closing File --------------------------\n')

def CloneFile(path,fil,tree_names,y_pred,var_name='score',ntup_opt='recreate',same_path=False):
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
    fil_new = fil.replace('.root','_clone.root')
    if not same_path:
        fil_new = path+fil[fil.rfind('/')+1:].replace('.root','_clone.root')
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
    print ('Closing File --------------------------\n')
    return fil_new

def SaveToNtuple(fil,var,varSpec,x1,x1spec,y1,y1_pred,w1,x2,x2spec,y2,y2_pred,w2):

    noo=1
    if isinstance(y1[0],list):
        noo=len(y1[0])
    print('\nNumber of Outputs:',noo)

    variable_size = len(var)
    variableSpec_size = len(varSpec)

    class_id = vector('int')()
    variables = variable_size*[0]
    variablesSpec = variableSpec_size*[0]
    for i in range(variable_size):
        variables[i] = array('f', [ 0. ] )
    for i in range(variableSpec_size):
        variablesSpec[i] = array('f', [ 0. ] )
    weight = array('f', [ 0. ] )
    score = vector('float')()

    f = TFile(fil,'recreate')
    t1 = TTree('tr_train','Tree for Training Output')
    t2 = TTree('tr_test','Tree for Test Output')

    t1.Branch('class_id',class_id)
    for i in range(variable_size):
        t1.Branch(var[i],variables[i],var[i]+'/F')
    for i in range(variableSpec_size):
        t1.Branch(varSpec[i],variablesSpec[i],varSpec[i]+'/F')
    t1.Branch('weight',weight,'weight/F')
    t1.Branch('score',score)

    t2.Branch('class_id',class_id)
    for i in range(variable_size):
        t2.Branch(var[i],variables[i],var[i]+'/F')
    for i in range(variableSpec_size):
        t2.Branch(varSpec[i],variablesSpec[i],varSpec[i]+'/F')
    t2.Branch('weight',weight,'weight/F')
    t2.Branch('score',score)

    for i in range(len(x1)):
        class_id.clear()
        if isinstance(y1[i],list):
            for y in y1[i]: class_id.push_back(int(y))
        else:
            class_id.push_back(int(y1[i]))
        for j in range(variable_size):
            variables[j][0] = x1[i][j]
        for j in range(variableSpec_size):
            variablesSpec[j][0] = x1spec[i][j]
        weight[0] = w1[i]
        score.clear()
        if isinstance(y1_pred[i],list):
            for y in y1_pred[i]: score.push_back(y)
        else:
            score.push_back(y1_pred[i])
        t1.Fill()
    t1.Write()

    for i in range(len(x2)):
        class_id.clear()
        if isinstance(y2[i],list):
            for y in y2[i]: class_id.push_back(int(y))
        else:
            class_id.push_back(int(y2[i]))
        for j in range(variable_size):
            variables[j][0] = x2[i][j]
        for j in range(variableSpec_size):
            variablesSpec[j][0] = x2spec[i][j]
        weight[0] = w2[i]
        score.clear()
        if isinstance(y2_pred[i],list):
            for y in y2_pred[i]: score.push_back(y)
        else:
            score.push_back(y2_pred[i])
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
from sklearn.externals.joblib import dump,load
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

def AddDropout(opt,Dropout=0.3):
    new_string = ''
    opt_split = opt.split(',')
    for i in range(len(opt_split)-1):
        new_string+=opt_split[i]+';'+str(Dropout)+','

    new_string+=opt_split[-1]

    print ('\nArchitecture expression with dropout: %s'%(new_string))
    return new_string

def predict_with_uncertainty(x, model, n_iter=10):
    import keras.backend as K
    
    f = K.function([model.layers[0].input, K.learning_phase()],
               [model.layers[-1].output])
    
    result = []

    print ('%i iteration will be propagated for uncertainty calculation'%(n_iter))
    
    for i in range(n_iter):
        result.append(f([x, 1]))

    result = np.array(result)

    prediction = result.mean(axis=0)
    uncertainty = result.var(axis=0)
    return prediction[0], uncertainty[0]

def RemoveSpecialChars(string):
    chars = [':','.','(',')','"','=','/',';',' ','|',',']
    for c in chars:
        string = string.replace(c,'_')
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
        if argv[i] == '--prepared-ntup' or argv[i] == '--clone-ntup':
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
