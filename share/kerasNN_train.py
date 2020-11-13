#!/usr/bin/env python

from TMVAFunc import *
from TMVATool import TMVATool
import sys,os
import numpy as np
from scipy.sparse import coo_matrix

from sklearn.utils import shuffle
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn import metrics
from sklearn.naive_bayes import GaussianNB
from sklearn.pipeline import make_pipeline

from sklearn.externals.joblib import dump, load

argv = sys.argv
argc = len(sys.argv)

#configuration
tool = TMVATool('Training')
tool.Parser(argc,argv)
tool.ReadConf()
tool.SetEvents()

#directory to store 
if not os.path.exists('./keras_output'):
    os.makedirs('./keras_output')
#directory to store 
if not os.path.exists('./keras_output/feature_weight'):
    os.makedirs('./keras_output/feature_weight')
#directory to store 
if not os.path.exists('./keras_output/model'):
    os.makedirs('./keras_output/model')
#loop for n fold
from keras.models import Sequential
for i in range(tool.GetNSplit()):
    #obtain data
    x_train,xspec_train,y_train,w_train=tool.GetEvents('Training',i)
    x_test,xspec_test,y_test,w_test=tool.GetEvents('Testing',i)

    #arrange data
    x_train,y_train,w_train = shuffle(x_train,y_train,w_train,random_state=0);
    ScaleWeights(y_train,w_train)
    # Fit to data and predict using pipelined scaling, GNB and PCA.
    std_clf = make_pipeline(StandardScaler(), PCA(n_components=tool.NVar), GaussianNB())
    std_clf.fit(x_train, y_train)
    pred_test_std = std_clf.predict(x_test)
    print('\nPrediction accuracy for the standardized test dataset with PCA')
    print('{:.2%}\n'.format(metrics.accuracy_score(y_test, pred_test_std)))
    pca_std = std_clf.named_steps['pca']
    print('\nPC 1 with scaling:\n', pca_std.components_[0])
    scaler = std_clf.named_steps['standardscaler']
    x_train_scaled = pca_std.transform(scaler.transform(x_train))
    x_test_scaled = pca_std.transform(scaler.transform(x_test))

    print (x_train_scaled)
    #train data
    model = GetKerasModel(tool.NVar,tool.GetArchitectureOpt())
    TrainKerasModel(model,tool.GetEngineOpt(),x_train_scaled,y_train,w_train)
    ypred_train = model.predict(x_train_scaled)
    ypred_test = model.predict(x_test_scaled)

    #save everything
    dump(scaler, 'keras_output/feature_weight/std_scaler_'+str(i)+'.bin', compress=True)
    dump(pca_std, 'keras_output/feature_weight/pca_'+str(i)+'.bin', compress=True)
    model.save('keras_output/model/model_'+str(i)+'.h5')
    model.save_weights('keras_output/model/weight_'+str(i)+'.h5')
    SaveToNtuple('keras_output/train_'+str(i)+'.root',CorrectNames(tool.Variables),CorrectNames(tool.SpecVariables)
                 ,x_train,xspec_train,y_train,ypred_train,w_train
                 ,x_test,xspec_test,y_test,ypred_test,w_test
    )
