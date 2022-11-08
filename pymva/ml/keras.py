import numpy as np
import pandas as pd

def ExponentialLoss():
    from tensorflow.keras import backend as K
    def loss(y_true, y_pred):
        return K.mean((y_true * K.exp(-0.5*y_pred)) + ((1.0-y_true) * K.exp(0.5*y_pred)),axis=-1)
    return loss

def ExponentialLoss_Metric(y_true, y_pred):
    from tensorflow.keras import backend as K
    return K.mean((-0.5 * y_true * K.exp(-0.5*y_pred)) + (0.5 * (1.0-y_true) * K.exp(0.5*y_pred)),axis=-1)

def GetKerasModel(input_dim,opt):
    from tensorflow.keras.models import Sequential
    from tensorflow.keras.layers import Dense, Dropout
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

def TrainKerasModel(path,model,opt,x,y,w,custom_callbacks=None):
    from tensorflow.keras.optimizers import SGD,Adam,RMSprop,schedules
    from tensorflow.keras.models import Sequential

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

    lrate_dec=0
    lrate_step=0
    if 'LRATE_DEC' in optlist: lrate_dec=float(optlist['LRATE_DEC'])
    if 'LRATE_STEP' in optlist: lrate_step=int(optlist['LRATE_STEP'])

    #Metrics
    mets=[]
    #if loss == 'Exp': mets.append(ExponentialLoss_Metric)

    #Callbacks
    cbacks=[]
    if custom_callbacks != None: cbacks.append(custom_callbacks) 

    if loss == 'Exp':
        loss = ExponentialLoss()

    if lrate_dec>0 and lrate_step>0:
        lrate = schedules.ExponentialDecay(initial_learning_rate=float(optlist['LRATE']),
                                 decay_steps=lrate_step,
                                 decay_rate=lrate_dec)
    print("Learning rate sceduled with initial rate "+str(float(optlist['LRATE']))+" by "+str(lrate_dec)+" decay and for each "+str(lrate_step)+" step.")

    optim=None
    if optimizer.upper() == 'SGD':
        optim=SGD(learning_rate=lrate, momentum=momentum)
    elif optimizer.upper() == 'ADAM':
        optim=Adam(learning_rate=lrate)
    elif optimizer.upper() == 'RMSPROP':
        optim=RMSprop(learning_rate=lrate, momentum=momentum)
    else:
        print ('Available optimizers: SGD,ADAM,RMSPROP')
        print ('You choose none, SGD will be run..')
        optim=SGD(learning_rate=lrate, momentum=momentum)
    
    model.compile(loss=loss, optimizer=optim, metrics=mets+['acc'])
    print (model.summary())
    model.fit(np.array(x).astype(np.float32), np.array(y).astype(np.float32), sample_weight = np.array(w).astype(np.float32), epochs=nepoch, batch_size=batch, callbacks=cbacks)

    
def CompileKerasModel(model,opt):
    from tensorflow.keras.optimizers import SGD,Adam,RMSprop,schedules
    from tensorflow.keras.models import Sequential
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

def SaveKerasModel(model,fil):
    from tensorflow.keras.optimizers import SGD,Adam,RMSprop,schedules
    from tensorflow.keras.models import Sequential
    import tensorflow as tf
    ind=0
    with open(fil, 'w') as fout:
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

def AddDropout(opt,Dropout=0.3):
    new_string = ''
    opt_split = opt.split(',')
    for i in range(len(opt_split)-1):
        new_string+=opt_split[i]+';'+str(Dropout)+','

    new_string+=opt_split[-1]

    print ('\nArchitecture expression with dropout: %s'%(new_string))
    return new_string

def predict_with_uncertainty(x, model, n_iter=10):
    from tensorflow.python.keras import backend as K
    from tensorflow.python.keras.backend import eager_learning_phase_scope

    f = K.function([model.input],
               [model.layers[-1].output])
    
    result = []

    print ('%i iteration will be propagated for uncertainty calculation'%(n_iter))
    
    for i in range(n_iter):
        #Run in training mode for dropout
        with eager_learning_phase_scope(value=1):
            result.append(f([x]))

    result = np.array(result)

    prediction = result.mean(axis=0)
    uncertainty = result.var(axis=0)
    return prediction[0], uncertainty[0]
