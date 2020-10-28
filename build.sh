#!bin/sh

make clean;make

location="$(cd -P "$(dirname "${_src}")" && pwd)"
export APP_HOME=$location
echo "APP Location : " ${APP_HOME}
export PATH=${PATH}${PATH:+:}${APP_HOME}
