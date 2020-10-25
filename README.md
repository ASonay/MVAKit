## Introduction

The project is aiming control to TMVA package by configuration files and commands in order to perform non-parameterized/parameterized binary or multiclass classification through different classifiers that TMVA tool provide.  

The basic usage of the tool;

**Setup environment :**    
```
source setup.sh
```

**Compiling :**    
```
source build.sh
```

**Running :**  
```
./tmva_trainer --ntup <ntup1.root> label1 --ntup <ntup2.root> label2 --ConfFile <ConfFile.txt>
```

Root files can be registered by using wildcards to different labels and labels can be created as many as needed.  

Example run;
```
./tmva_trainer --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttH_tttt_m400_MgPy8_mc16* Signal --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttbar_PhPy8_AFII_AllFilt_mc16* Bkg --conf config/config_bdt.conf
```

This will run the binary classification by the BDT classifier depending on the selections defined in configuration file.

## Configuration File

Following arguments are used inside the configuration file for controlling the tool,  

* `TrainingOpt:` This option is used on [BookMethod()](https://root.cern/doc/master/classTMVA_1_1Factory.html#a35c42e83410f857150bb2c150bb97474) as an option of defined classifier.  
Default option:
```
!H:!V:NTrees=600:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.6:SeparationType=GiniIndex:nCuts=30:MaxDepth=2:NegWeightTreatment=IgnoreNegWeightsInTraining
```

* `FactoryOpt:` [TMVA::Factory()](https://root.cern/doc/master/classTMVA_1_1Factory.html) option for the job configuration.  
Default option:
```
!V:!Silent:Transformations=I;D;P;G,D:AnalysisType=Classification
```

* `SamplingOpt:` Sample option for configuration of [TMVA::DAtaLoader()](https://root.cern/doc/master/classTMVA_1_1DataLoader.html).  
Default option:
```
nTrain_Signal=0:nTrain_Background=0:NormMode=EqualNumEvents:V
```

* `ClassifierOpt:` Classifier selection (e.g. `DNN`, `BDT`).  
Default option:
```
BDT
```

* `Var:` The variable for training.

* `OtherVar:` The variable to be added into ROOT tree, not to be used on the training (Spectator variable).

* `ParamVar:` The variable for parameterized training. The ntuple file will be parameterized has to contain the name of this variable before the parameter value (see [Parameterized Training](#parameterized-training)).

* `SigWeight:` Signal samples weight expression. (The phrase before `Weight:` refer the label and should be comparable with label.)

* `BkgWeight:` Background samples weight expression. (The phrase before `Weight:` refer the label and should be comparable with label.)

* `SigCut:` The cut will be applied for signal samples. (The phrase before `Cut:` refer the label and should be comparable with label.)

* `BkgCut:` The cut will be applied for background samples. (The phrase before `Cut:` refer the label and should be comparable with label.)

* `Split:` Splitting option for categorizing events as training or testing samples (see [Splitting the Samples](#splitting-the-samples)).

## Splitting the Samples

Two methods are available for splitting samples.  

* `Split:x`, where the x is percentage of training samples and rest will be the testing samples (e.g. Split:50).

* The second method (recommended way) is selection from tree (e.g. event id, eventNumber%2==0 will select training samples if the event number is even, and odd numbers will be the testing samples).

#### Cross Validation

If comma used between the splitting option that given above, training will be run for each splitting option for cross terms (e.g. `Split: eventNumber%2==0,eventNumber%2==1` In the first training, the training samples will be filled for even numbers and odd numbers will be test sample and for the second training this will be opposite).

## Parameterized Training

The only thing you should do for parameterizing training is specifying `ParamVar: x`; where x is a string, which must be contained by the ntuple file, which will be used for parameterization. The parameters will be obtained from the ntuple name, so it is expected to ntuple name contains those parameters and the name given to variable should be some part before the parameter in ntuple name (e.g. for the `ttH_tttt_m400_x.root` root file, variable name should be, `ParamVar: ttH_tttt_m`, then parameter will be 400.). The parameters will be filled for the rest of the samples based on the distribution of original file.

Example run;  
```
./tmva_trainer --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttH_tttt_m*_MgPy8_mc16* Signal --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttbar_PhPy8_AFII_AllFilt_mc16* Bkg --conf config/config_massparam_bdt.conf
```

## Multiclass Training

By the different labeling of the declated ntuples, multiclass training is possible if proper options are given in the configuration file.

Example run;  
```
./tmva_trainer --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttH_tttt_m*_mc16a.root Signal --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttbar_PhPy8_AFII_AllFilt_mc16a.root Bkg1 --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttW_Sherpa_mc16a.root Bkg2 --conf config/config_massparam_mclass_DNN.conf
```

Contact : @asonay