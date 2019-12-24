#!/bin/sh

#build switch project
make -C ./NES-Switch

#build desktop project
cmake -S ./NES-Desktop -B ./NES-Desktop/build
make -C ./NES-Desktop/build

#build logging server
cmake -S ./log_server -B ./log_server/build
make -C ./log_server/build

#upload via ftp
if [ -z "$1" ]
    then
    echo "no IP address supplied."
    echo "Finished building."
    exit 1
fi

if [ -z "$2" ]
    then
    echo "no port supplied. Please provide the port to ftp over."
    exit 1
fi

HOST=$1
PORT=$2
FILE='switch.nro'

gftp -n $HOST $PORT<<END_SCRIPT
type binary
put $PWD/switch/$FILE /switch/$FILE
quit
END_SCRIPT

#launch server
./desktop/build/server