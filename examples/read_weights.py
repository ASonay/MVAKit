#! /usr/bin/env python

import sys, os, argparse
import uproot

import ROOT
ROOT.gSystem.Load("${MVAKIT_HOME}/build/lib/libMVAKit.so")
ROOT.gROOT.LoadMacro("${MVAKIT_HOME}/scripts/GetTMVAScore.cpp")
ROOT.gROOT.LoadMacro("${MVAKIT_HOME}/scripts/preReweighting.cpp")

#v1:nRCJetsM100;v2:nJets;v3:jet_pt[0];v4:jet_pt[1];v5:jet_pt[2];v6:jet_pt[3];v7:jet_pt[4];v8:Alt$(jet_pt[5],0);v9:Alt$(jet_pt[6],0);v10:nMuons+nElectrons==2?Sum$(jet_pt*(Iteration$>=7)):Alt$(jet_pt[7],0);v11:nMuons+nElectrons==2?Alt$(el_pt[0],0)+Alt$(mu_pt[0],0):Alt$(jet_pt[8],0);v12:nMuons+nElectrons==2?met_met:Sum$(jet_pt*(Iteration$>=9));v13:nMuons+nElectrons==2?dRjj_Avg:Alt$(el_pt[0],0)+Alt$(mu_pt[0],0);v14:met_met;v15:dRjj_Avg
#rew:GetNNRew(mcChannelNumber,nMuons+nElectrons,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15)

# Use this in case ROOT package used
def SetAlias(tree):
    aliases = [
        ("v1","nRCJetsM100"),
        ("v2","nJets"),
        ("v3","jet_pt[0]"),
        ("v4","jet_pt[1]"),
        ("v5","jet_pt[2]"),
        ("v6","jet_pt[3]"),
        ("v7","jet_pt[4]"),
        ("v8","Alt$(jet_pt[5],0)"),
        ("v9","Alt$(jet_pt[6],0)"),
        ("v10","nMuons+nElectrons==2?Sum$(jet_pt*(Iteration$>=7)):Alt$(jet_pt[7],0)"),
        ("v11","nMuons+nElectrons==2?Alt$(el_pt[0],0)+Alt$(mu_pt[0],0):Alt$(jet_pt[8],0)"),
        ("v12","nMuons+nElectrons==2?met_met:Sum$(jet_pt*(Iteration$>=9))"),
        ("v13","nMuons+nElectrons==2?dRjj_Avg:Alt$(el_pt[0],0)+Alt$(mu_pt[0],0)"),
        ("v14","met_met"),
        ("v15","dRjj_Avg")
        ]
    for alias in aliases:
        tree.SetAlias(alias[0],alias[1])

def CalculateFromTree(tr):
    channel = tr.mcChannelNumber
    lep = tr.nMuons+tr.nElectrons
    nrc = tr.nRCJetsM100
    nj = tr.nJets
    jets = tr.jet_pt

    jets8 = jets[7] if len(jets) > 7 else 0
    jets9 = jets[8] if len(jets) > 8 else 0
    jets10 = sum(jets[9:]) if len(jets) > 9 else 0
    
    return ROOT.GetNNRew(channel,lep,
                         nrc,nj,
                         jets[0],jets[1],jets[2],jets[3],jets[4],jets[5],jets[6],
                         jets8,jets9,jets10,
                         tr.met_met,tr.dRjj_Avg
                         )


if __name__ == "__main__":

    ROOT.GetTMVAScore(400,2,0)

    parser = argparse.ArgumentParser(description='Reading reweighting factor')
    parser.add_argument("-i", "--input", dest='input_file', default='',
                    help='Input ntuple.')
    parser.add_argument("-t", "--tree", dest='input_tree', default='',
                    help='Input ttree.')

    args = parser.parse_args()

    tfile = ROOT.TFile(args.input_file)
    ttree = tfile.Get(args.input_tree)
    nEntries = ttree.GetEntries()
    
    for i in range(0, 2):#nEntries):
        ttree.GetEntry(i)
        print( CalculateFromTree(ttree) )
