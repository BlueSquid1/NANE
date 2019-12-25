#!/bin/sh
NES_SWITCH_PATH="NANE-Switch"
NES_DESKTOP_PATH="NANE-Desktop"
LOGGING_SERVER_PATH="LogServer"
LOGGING_SERVER_BINARY="LogServer"
NES_SWITCH_BINARY="NANE.nro"
LOG_NETWORK_ADAPTER="en0" #or eth0 or wan0

#get local ip address
LOGGING_SERVER_IP=$(ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1')

echo "LOGGING IP address = $LOGGING_SERVER_IP"

#build switch project
make -C ./$NES_SWITCH_PATH LOG_SERVER_IP=$LOGGING_SERVER_IP

#build desktop project
cmake -S ./$NES_DESKTOP_PATH -B ./$NES_DESKTOP_PATH/build -D LOG_SERVER_IP=$LOGGING_SERVER_IP
make -C ./$NES_DESKTOP_PATH/build

#build logging server
cmake -S ./$LOGGING_SERVER_PATH -B ./$LOGGING_SERVER_PATH/build
make -C ./$LOGGING_SERVER_PATH/build

#upload via ftp
CACHE_FILE=cache.ini

#get cache value if it exists
if [ -f $CACHE_FILE ]; then
    echo "found a cache file. Loading cache"
    source $CACHE_FILE

    #see if ip address arguement
    if [ ! -z "$1" ]; then
        #see if cache needs to be updated
        if [ $HOST != $1 ]; then
            echo "cache value is out of date with new log server ip address. Updating cache."
            rm $CACHE_FILE
            HOST=$1
            declare -p HOST | cut -d ' ' -f 3- >> $CACHE_FILE
        fi
    fi
else
    #cache file does not exist
    if [ -z "$1" ]; then
        echo "no ip address stored in the cache file"
        echo "Finished building"
        exit 1
    fi

    HOST=$1
    echo "no cache file found. storing logger server ip address: $HOST to cache"

    #create cache file
    declare -p HOST | cut -d ' ' -f 3- >> $CACHE_FILE
fi

PORT=5000
FILE=$NES_SWITCH_BINARY
echo "uploading $PWD/$NES_SWITCH_PATH/$FILE to $HOST:$PORT/switch/$FILE"
gftp -n $HOST $PORT<<END_SCRIPT
type binary
put $PWD/$NES_SWITCH_PATH/$FILE /switch/$FILE
quit
END_SCRIPT

#launch server
echo "launch log server"
./$LOGGING_SERVER_PATH/build/$LOGGING_SERVER_BINARY