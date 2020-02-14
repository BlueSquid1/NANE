#!/bin/sh

while getopts ":b:t:p:" opt; do
  case $opt in
    b) BuiltType="$OPTARG" #optional, Release or Debug (defaults to Release)
    ;;
    t) DeployTarget="$OPTARG" #optional, values can be "switch" or "desktop"
    ;;
    p) SwitchIpAddress="$OPTARG" #optional, value of the form "xxx.xxx.xxx.xxx" for example: "192.168.1.100"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    ;;
  esac
done

#if its not specified
if [[ -z "$BuiltType" || $BuiltType != "Debug" ]]; then
    BuiltType="Release"
fi

echo "deploying with:"
echo "BuiltType=$BuiltType"
echo "DeployTarget=$DeployTarget" 
echo "SwitchIpAddress=$SwitchIpAddress"

#BuiltType=$1 #optional, release or debug (defaults to release)
#DeployTarget=$2 #optional, values can be "switch" or "desktop"
#SwitchIpAddress=$3 #optional, value of the form "xxx.xxx.xxx.xxx" for example: "192.168.1.100"

NES_SWITCH_PATH="NANE-Switch"
NES_DESKTOP_PATH="NANE-Desktop"
LOGGING_SERVER_PATH="LogServer"

NES_SWITCH_BINARY="NANE.nro"
NES_DESKTOP_BINARY="NANE"
NES_DESKTOP_UNIT_TESTS_BINARY="unit_tests"
LOGGING_SERVER_BINARY="LogServer"

LOG_NETWORK_ADAPTER="en0" #or eth0 or wan0

#get local ip address
LOGGING_SERVER_IP=$(ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1')
echo "LOGGING IP address = $LOGGING_SERVER_IP"

#build switch project
#make -j4 -C ./$NES_SWITCH_PATH LOG_SERVER_IP=$LOGGING_SERVER_IP
#ret=$?
#if [[ $ret != 0 ]]; then
#    echo "Failed to build switch binary. Exiting."
#    exit 1
#fi

#build desktop project
cmake -DCMAKE_BUILD_TYPE="$BuiltType" -S ./$NES_DESKTOP_PATH -B ./$NES_DESKTOP_PATH/build/$BuiltType
make -j4 -C ./$NES_DESKTOP_PATH/build/$BuiltType
ret=$?
if [[ $ret != 0 ]]; then
    echo "Failed to build desktop binary. Exiting."
    exit 1
fi

#build logging server
cmake -DCMAKE_BUILD_TYPE="Release" -S ./$LOGGING_SERVER_PATH -B ./$LOGGING_SERVER_PATH/build/Release
make -j4 -C ./$LOGGING_SERVER_PATH/build/Release
ret=$?
if [[ $ret != 0 ]]; then
    echo "Failed to build log server. Exiting."
    exit 1
fi

#run unit tests
echo "running unit tests"
./$NES_DESKTOP_PATH/build/$BuiltType/$NES_DESKTOP_UNIT_TESTS_BINARY
ret=$?
if [[ $ret != 0 ]]; then
    echo "not all unit test passed. Exiting."
    exit 1
fi

if [[ $DeployTarget == "desktop" ]]; then
    echo "launching NANE for the desktop"
    ./$NES_DESKTOP_PATH/build/$BuiltType/$NES_DESKTOP_BINARY
    exit 0

elif [[ $DeployTarget == "switch" ]]; then
    #upload via ftp
    CACHE_FILE=cache.ini

    #get cache value if it exists
    if [[ -f $CACHE_FILE ]]; then
        echo "found a cache file. Loading cache"
        source $CACHE_FILE

        #see if ip address arguement
        if [[ ! -z "$SwitchIpAddress" ]]; then
            #see if cache needs to be updated
            if [[ $HOST != $SwitchIpAddress ]]; then
                echo "cache value is out of date with new log server ip address. Updating cache."
                rm $CACHE_FILE
                HOST=$1
                declare -p HOST | cut -d ' ' -f 3- >> $CACHE_FILE
            fi
        fi
    else
        #cache file does not exist
        if [[ -z "$SwitchIpAddress" ]]; then
            echo "no ip address stored in the cache file"
            echo "Finished building"
            exit 0
        fi

        HOST=$SwitchIpAddress
        echo "no cache file found. storing logger server ip address: $HOST to cache"

        #create cache file
        declare -p HOST | cut -d ' ' -f 3- >> $CACHE_FILE
    fi

    #uploading switch binary to switch via FTP
    PORT=5000
    FILE=$NES_SWITCH_BINARY
    echo "uploading $PWD/$NES_SWITCH_PATH/$FILE to $HOST:$PORT/switch/$FILE"
    gftp -v -n $HOST $PORT<<END_SCRIPT
type binary
put $PWD/$NES_SWITCH_PATH/$FILE /switch/$FILE
quit
END_SCRIPT


    #launch server
    echo "launching log server"
    ./$LOGGING_SERVER_PATH/build/Release/$LOGGING_SERVER_BINARY
    ret=$?
    if [[ $ret != 0 ]]; then
        echo "Failed to launch log server. Exiting."
        exit 1
    fi
else
    echo "finished building"
fi