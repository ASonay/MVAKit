## Introduction

The project is aiming control to TMVA package by configuration files and commands in order to perform non-parameterized/parameterized binary or multiclass classification through different classifiers that TMVA tool provide.  

The basic usage of the tool;

**Setup environment :**    
```
source setup.sh
```

**Compiling :**    
- First time: `tmvatool-build`
- Recompile: `tmvatool-make`
- Clean folder: `tmvatool-clean`

**Running :**
* Training,
```
tmva_train --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --ConfFile <ConfFile.txt>
```
* Evaluating,
```
tmva_eval --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --xml <file1.xml>,..,<filen.xml> <cond1>,..,<condn> --ConfFile <ConfFile.txt> --par <value> --var <Variable Name>
```

, where the `--par` and the `--var` optional for value of the parameterized training and variable name to be recorded in ntuple, respectively.


Root files can be registered by using wildcards to different labels and labels can be created as many as needed.  

Example run;
```
tmva_train --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttH_tttt_m400_MgPy8_mc16* Signal --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttbar_PhPy8_AFII_AllFilt_mc16* Bkg --conf config/config_bdt.conf
```

This will run the binary classification by the BDT classifier depending on the selections defined in configuration file.

# Training

This tool aim to perform a training by using the TMVA software and control it wit the configuration file that will be explained below.

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
* `AllTree:` Specifies tree name for all samples. By changing `All` with any sample label, different tree name can be specified for different ntuples.

* `Var:` The variable for training.

* `OtherVar:` The variable to be added into ROOT tree, not to be used on the training (Spectator variable).

* `ParamVar:` The variable for parameterized training. The ntuple file will be parameterized has to contain the name of this variable before the parameter value (see [Parameterized Training](#parameterized-training)).

  * `ParamSplit: (Parameterization specific)` This option specify the splitting for the samples that is not contains parameter. This could be based on weighted distribution of each parameter by "XS"/"Cross-section" (default) or based on number of events in each parameter by "Enum"/"Event-number" or it could be uniformly selected for each parameter by "Uni"/"Uniform" option.
  
  * `ParamScale (Parameterization specific)` This option will scale the samples that is not contains parameter to based on weighted distribution of parameters by "XS"/"Cross-section", or it will simply scale all the parameters to be flat by "Flat" option.

* `<label>Weight:` Weight expression. (The phrase before `Weight:` refer the label and should be comparable with the label defined after ntuples.)

* `<label>Scale:` Scaling factor for selected sample. It could be define for different selection (e.g. cut1;value1,...cutN;valueN) or different parameterization (e.g. param1;value1,..,param2;value2), the difference between selection and parameter, parameter must be digit that you have in the ntuple name that will be parameterized.. (The phrase before `Scale:` refer the label and should be comparable with the label defined after ntuples.)

* `<label>Cut:` The cut will be applied for the defined samples. (The phrase before `Cut:` refer the label and should be comparable with the label defined after ntuples.)

* `Split:` Splitting option for categorizing events as training or testing samples (see [Splitting the Samples](#splitting-the-samples)).

* `LoadFiles:` Loading custom file for using user specified function. Multiple file can be loaded by comma separation.

* `LoadLibs:` Loading libraries if needed. Multiple library can be loaded by comma separation.

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
tmva_train --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttH_tttt_m*_MgPy8_mc16* Signal --ntup /eos/user/b/bsm4tops/bsm4teos/BSM4t-2121200-1LOS-v1/ljets{9,ge10}j/ttbar_PhPy8_AFII_AllFilt_mc16* Bkg --conf config/config_massparam_bdt.conf
```

## Multiclass Training

By the different labeling of the declared ntuples, multiclass training is possible if proper options are given in the configuration file.

Example run;  
```
tmva_train --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttH_tttt_m*_mc16a.root Signal --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttbar_PhPy8_AFII_AllFilt_mc16a.root Bkg1 --ntup /eos/atlas/atlascerngroupdisk/phys-top/4tops2019/ntuples/common-fw_tag212750/offline/SM4t-212750-1LOS-v3/ljetsge10j/ttW_Sherpa_mc16a.root Bkg2 --conf config/config_massparam_mclass_DNN.conf
```

# Evaluating Results

This section is explaining to evaluating results that you have done their trainings. Simply, following tool works for filling your existing ntuples with the TMVA score. Although it is working, it is still under development.
```
tmva_eval --ntup <ntup1.root> label1 .. --ntup <ntupn.root> labeln --xml <file1.xml>,..,<filen.xml> <cond1>,..,<condn> --ConfFile <ConfFile.txt> --par <value> --var <Variable Name>
```
For the cross validation, conditions has to be given in opposite way. Although it is working for multiple samples, the suggested way is using it for one ntuple at once (memory usage will be optimized later).

The suggested way to evaluate the score is using the external script (e.g. [scripts/GetTMVAScore.cpp](scripts/GetTMVAScore.cpp)).

# Python Extension (Will be updated)

Contact : @asonay