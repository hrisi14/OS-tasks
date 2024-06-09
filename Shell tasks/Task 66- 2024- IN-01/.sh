#!/bin/bash

if [[ -z "${ARKCONF}" ]] ; then
        echo "Error! Name of config file has not been specified!"
        exit 1
fi

if [[ "${#}" -lt 1 ]] ;  then
        echo "Error! At leats push/pull option must be specified!"
        exit 1
fi

CONFIG_FILE_NAME=$(echo "${ARKCONF}")
SERVER='' #in case it is empty, iterate through the config file servers
SYNC_DIRECTION=''  #push/pull- specification
ABS_FLAG=false

SYNC_DIRECTION=$(echo "${*}" | sed -E 's/ /\n/g' |grep -E '^(push|pull)$')
if [[ -z "${SYNC_DIRECTION}" ]] ; then
        echo "Error! An obligatory parameter has not been specified!"
        exit 1
fi

#server catching
SERVER=$(echo "${*}" | sed -E 's/ /\n/g' | grep -E '^a[a-z]+$')

#Flags' consideration
FLAGS_TEMP=$(mktemp)

echo "$(echo "${*}" | sed -E 's/ /\n/g' | grep -E '^-[a-z]$')" > "${FLAGS_TEMP}"

if [[ ! -z "$(echo "${FLAGS_TEMP}" | grep -E '^-a$' )" ]] ; then
        ABS_FLAG=true
fi

OPTIONS=$(echo "${FLAGS_TEMP}" | sed 's/\n/ /g') 


DIR_TO_SYNC=$(echo "${CONGIG_FILE_NAME}" | grep -E '^WHAT=\"*\"$' | cut -d '=' -f 2| sed -E 's/"//g')

if [[ -z "${SERVER}" ]] ; then
        SERVER=$(echo "${CONFIG_FILE_NAME}" | grep -E '^WHERE=\"*\" $' | cut -d '=' -f 2 | sed -E 's/"//g')

if [[ -z "${SERVER}" ]] ; then
        echo "Error! No servers specified!"
        exit 1
fi

USER=$(echo "${CONFIG_FILE_NAME}" | grep -E '^WHO=\"*\"$' | cut -d '=' -f 2 | sed -E 's/"//g')

#The real synchronization:

#Pull sync
if [[ "${DIR_TO_SYNC}" == "push" ]] ; then
        while IFS= read -rd $'\0' server; do
                read -p "${DIR_TO_SYNC} and ${sever} would be synchronized. Type yes or no"ans
                if [[ ${ans}=="yes" ]] ; then

                if [[ -z "${USER}" ]] ; then
                        rsync "${OPTIONS}" "${DIR_TO_SYNC}" "${server}"
                else
                        rsync "${OPTIONS}" "${DIR_TO_SYNC}"  "${USER}"@server:"${server}"
                fi
                fi
        done < "${SERVER}"
fi

if [[ "${DIR_TO_SYNC}"=="pull" ]] ; then
        while IFS= read -rd $'\0' server ; do
                read -p "${DIR_TO_SYNC} and ${server} would be synchronized. Type yes or no" ans
                if [["${ans}"=="yes"]] ; then
                if [[ -z "${USER}" ]] ; then
                        rsync "${OPTIONS}" "${server}" "${DIR_TO_SYNC}"
                else
                        rsync "${OPTIONS}" "${USER}"@server:"${server}" "${DIR_TO_SYNC}"
                fi
                fi
        done < "${SERVER}"
fi


 > "${FLAGS_TEMP}"

rm -f ${FLAGS_TEMP}
