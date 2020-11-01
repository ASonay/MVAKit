#!bin/sh

if [ "${BASH_SOURCE[0]}" != "" ]; then
    # This should work in bash.
    _src=${BASH_SOURCE[0]}
elif [ "${ZSH_NAME}" != "" ]; then
    # And this in zsh.
    _src=${(%):-%x}
elif [ "${1}" != "" ]; then
    # If none of the above works, we take it from the command line.
    _src="${1/setup.sh/}/setup.sh"
else
    echo -e "\033[1;31mERROR:\033[0m Could not determine the base directory of TMVATool, i.e. where \"setup.sh\" is located."
    return 1
fi

location="$(cd -P "$(dirname "${_src}")" && pwd)"
unset _src

echo -e "\n*************************************************"
echo -e "*External tool to control TMVA by configuration.*"
echo -e "*Developed by Anil Sonay, anil.sonay@cern.ch    *"
echo -e "*************************************************\n"

export APP_HOME=$location
echo -e "\nAPP Home Dir : " ${APP_HOME}

#setup root and LCG environment
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh --quiet
LCG_setup="views LCG_97a x86_64-centos7-gcc8-opt"
echo -e "LCG setup will be used, " ${LCG_setup}
lsetup "${LCG_setup}"

alias tmvatool-make='cd build/; make -j4; cd ../'
alias tmvatool-clean='rm -rf build/'
alias tmvatool-build='mkdir build && cd build; cmake ../; make -j4; cd ../'

if [ -d "${APP_HOME}/build/bin" ] 
then
    echo -e "Your build/bin directory already exist."
    echo -e "\nTo recompile please use \e[1m\e[4mtmvatool-make\e[0m.\n"
else
    echo -e "\nTo compile please use \e[1m\e[4mtmvatool-build\e[0m.\n"
fi

    
export PATH=${PATH}${PATH:+:}${APP_HOME}/build/bin
