#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Wrong arg count!"
    exit 1
fi

if [[ ! -e "${1}" ]] ; then
        echo "Such a file does not exist!"
        exit 1
fi

if [[ ! -d "${1}" ]] ; then
        echo "First argument must be a directory!"
        exit 1
fi

if ! echo "${2}" | egrep -q '^[^0][0-9]+$' ; then
        echo "Second argument must be a number!"
        exit 1
fi

if [[ "${2}" -lt 1 || "${2}" -gt 99 ]] ; then
        echo "Wrong interval!"
        exit 1
fi

TMP_OBJECTS="$(mktemp)"
index=0
#remove all the broken symlinks from the initial directory

while read file ; do
        if [[ -L "${file}" ]] ; then
                if [[ ! -e "${file}" ]] ; then
                        unlink "${file}"
                fi
        else
                echo "${file}" >> "${TMP_OBJECTS}"
        fi
done < <(find "${1}" -type f)

cat "${TMP_OBJECTS}" | cut -d '-' -f 1,2 | sort | uniq > "${TMP_OBJECTS}"  #extracting all unique valid objects

TO_REMAIN="$(mktemp)"

while read object ; do
        annual="$(find "${1}/0" -type f |grep -E "^${object}-" | sort -t '-' -k 3 -n -r | head -n 1)"

        if [[ ! -z "${annual}" ]] ; then
                echo "${annual}" >> "${TO_REMAIN}" #capturing each object's annual backup
    fi

        if [[ "$(echo "$(find "${1}/1" -type f | grep -E "${object}" | wc -l)" )" -ge 2 ]] ; then
                find "${1}/1" -type f | grep -E "^${object}-" "${TMP1}" | head -n 2 >> "${TO_REMAIN}"
        fi

        if [[ "$(echo "$(find "${1}/2" -type f |grep -E "${object}" | wc -l)" )" -ge 3 ]] ; then
                find "${1}/2" -type f |grep -E "^${object}-" | head -n 3 >> "${TO_REMAIN}"
        fi

        if [[ "$( echo "$(find "${1}/3" -type f | grep -E "${object}" | wc -l)" )" -ge 4  ]] ; then
            find "${1}/3" -type f | grep -E "^${object}-" | head -n 4 >> "${TO_REMAIN}"
    fi
done < "${TMP_OBJECTS}"

while read file ; do
        if [[ "$(echo "$(df "${1}" | grep -E '[0-9]+%' | cut -d ' ' -f5)")" -le "${2}" ]] ; then
                break
        fi

        if ! grep -qE "^${file}$" "${TO_REMAIN}" ; then
                unlink "${file}"
        fi

done < <(find "${1}" -type f)

rm -f "${TO_REMAIN}"

#going through the initial dir and removing all the files with names that do not occur in the "TO_REMAIN"f file
