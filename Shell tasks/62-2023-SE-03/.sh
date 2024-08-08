
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
   echo "Wrong arg count!"
   exit 1
fi

filesCount="$(find "${1}" -type f | wc -l)"
TOTAL="$( echo "${filesCount}/2" | bc)"

#Algorithm:
#for each file find the words that are encountered >=3 times in it
#list them in a temp file
#traverse the words in the temp file and count the occurrences of each word in the dir's files
#select only the words that have been encountered >= TOTAL
#sort them by general occurrences in all files (first column, numerically)
#head the result

TEMP="$(mktemp)" #a list of words that occur >=3 in a file
TMP_RES="$(mktemp)"  #stopwords list

while read file ; do

         TMP_CONTENT="$(mktemp)" #list of uniq words to a file
     sed -E 's/\s+/\n/g' "${file}" | sed -E 's/[^a-z\n]//g' | sort | uniq > "${TMP_CONTENT}"
     while read word; do
                occurrences="$(grep -E -o "${word}"  "${file}" | wc -l)"

                if [[ "${occurrences}" -ge 3 ]] ; then
                        echo "${word}"  >> "${TEMP}"
                fi
    done < "${TMP_CONTENT}"

        rm -f "${TMP_CONTENT}"

done < <(find "${1}" -type f 2>/dev/null)

echo "$(sort "${TEMP}" | uniq -c)" > "${TMP_RES}"
echo "$(sort -r -n -k 1 -t ' ' "${TMP_RES}" | uniq)" > "${TMP_RES}"

cat "${TMP_RES}" | head -n 10

rm -f "${TEMP}"
rm -f "${TMP_RES}"
