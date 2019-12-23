#!/bin/sh

#build switch project
make -C ./switch

#build logging server
cmake -S ./desktop -B ./desktop/build
make -C ./desktop/build

#upload via ftp
if [ -z "$1" ]
    then
    echo "no IP address supplied. Please provide the IP address from the nintendo switch."
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