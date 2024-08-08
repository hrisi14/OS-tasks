
#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
        echo "Error! Wrong arguments count!"
        exit 1
fi

#if [[ ! -e "${1}" ]] ; then    
#       echo "Error! File does not exist!"
#       exit 1
#fi

if [[ ! -e "${2}" ]] ; then
        echo "There is no such directory!"
        exit 1
fi

if [[ ! -d "${2}" ]] ; then
        echo "Second arg must be a directory!"
        exit 1
fi

while read word ; do
        while read file ; do
                wordLen="$(echo -n "${word}" | wc -m)" #stands for the number of * to substitute the word with
                substitution=$(printf "%${wordLen}s" | tr ' ' '*')
        sed -i -E "s/\b${word}\b/${substitution}/Ig" "${file}"

        done < <(find "${2}" -type f -name '*.txt')
done < "${1}"
