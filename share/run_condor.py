#!/usr/bin/env python

import os,shutil,fileinput
import argparse
from argparse import RawTextHelpFormatter
from collections import defaultdict

def RemoveSpecialChars(string):
    chars = [':','.','(',')','"','=','/',';',' ']
    for c in chars:
        string = string.replace(c,'_')
    return string

class Job:
    def __init__ (self, environment, cmd):
        self.env = environment
        self.cmd = cmd
        self.path = os.getcwd()
        self.dirs = []
        self.command_lines = [""]

    def createEnvironment(self):
        dest = 'job_HTcondor'
        self.dirs.append(dest)
        if os.path.exists(dest):
            try:
                shutil.rmtree(dest)
            except OSError as e:
                print ('Error removing directory: %s : %s'%(dest,e.strerror))
                print ('This could cause problem on submitting HTCondor job')
            os.makedirs(dest)
        else:
            os.makedirs(dest)
        self.createCondorScript(dest,'nextweek')
        self.createBatchScript(dest,self.cmd)

    def createCondorScript(self,dest, queue):
        with open(dest+"/job.sub","w") as f:
            f.write('universe = vanilla\n')
            f.write('Notification    = Never\n')
            f.write("executable\t\t = "+dest+".sh\n")
            f.write("output\t\t = job.out\n")
            f.write("error\t\t = job.err\n")
            f.write("log\t\t = job.log\n")
            f.write('GetEnv          = True\n')
            f.write('+JobFlavour\t\t = "%s"\n'%queue)
            f.write('+Experiment     = "atlas"\n')
            f.write('+Job_Type       = "cas"\n')
            f.write("queue\n")
            f.close()
        
    def createBatchScript(self,dest,cmd):
        with open(dest+"/"+dest+".sh","w") as f:
            f.write("#!/bin/bash"+"\n")
            f.write('source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh\n')
            f.write('lsetup "%s"\n'%self.env)
            f.write(cmd+"\n")
            f.close()

    def submitJobs(self):
        for d in self.dirs:
            os.chdir(self.path+"/"+d)
            os.chmod(d+".sh", 0o777)
            print "\t > Submission of %s on the HTCondor" % (d+".sh")
            os.system("condor_submit job.sub")
        os.chdir(self.path)
            

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser("Submit jobs to HTCondor", formatter_class=RawTextHelpFormatter)
    parser.add_argument("-c","--command",action="store", dest="command", help="Command to run", required=True)
    parser.add_argument("-e","--environment",action="store", dest="environment", help="LCG Environment.", default="views LCG_97a x86_64-centos7-gcc8-opt", required=False)

    args = parser.parse_args()

    j=Job(args.environment,args.command)
    j.createEnvironment()
    j.submitJobs()
