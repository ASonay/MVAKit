#!bin/sh

make clean;make

export APP_HOME=$location
export PATH=${PATH}${PATH:+:}${APP_HOME}
