#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
echo "Wrong argument count!"
exit 1
fi

if [[ ! -d "${1}" ]] ; then
echo "Error! A directory needs to be passed as an argument!"
exit 1
fi


memorySize=0
groupsFlag=0
duplGroups=0

TEMPFILE="$(mktemp)"

while read FILE ; do

        echo "${FILE}" >> "${TEMPFILE}"

    while read CURRENT; do

        #echo "$(grep ${CURRENT} ${TEMPFILE} )"
                if [[ -z "$(grep   ${CURRENT}  ${TEMPFILE} )" ]] ; then
                    firstHash="$(sha256sum ${FILE} | awk '{print $1}' )"
                    secondHash="$(sha256sum ${CURRENT} | awk '{print $1}' )"

            #echo "${firstHash}"
            #echo "${secondHash}"

            if [[ "${firstHash}" == "${secondHash}" ]]; then
                currentFileSize="$(stat ${CURRENT} -c '%s')"
                memorySize="$((memorySize + ${currentFileSize}))"

        #Is this the correct way of solving this problem??!

                unlink "${CURRENT}"
                ln "${FILE}"  "${CURRENT}"  #Make the files point to the same data copy
                groupsFlag=1
                echo "${CURRENT}" >> "${TEMPFILE}"

           fi
           fi

#unifies hardlinks; deletes one of the files; increments ramCounter; increments deletedFilesCounter

    done < <(find "${1}" -type f)

    if [[ "${groupsFlag}" -eq 1 ]]; then
       duplGroups="$((duplGroups + 1))"
       groupsFlag=0
    fi
done < <(find "${1}" -type f)

echo "${duplGroups}"  #Check how to do it with printf!!!
echo "${memorySize}"

rm -f "${TEMPFILE}"
