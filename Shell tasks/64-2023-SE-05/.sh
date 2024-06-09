
#!/bin/bash

TEMP_FILE=$(mktemp)

total_lookups=0
found=true


while ${found}; do  #or without parenthesis or with quotes
        found=false
        total_lookups="$((total_lookups + 1))"

        while read my_command;  do
                command_rss="$(ps -e -o rss=,comm= | grep "${my_command}" | awk '{sum+=$1} END {print sum}')"

                if [[ "${command_rss}" -gt 65536 ]] ; then
                        found=true
                        echo "${my_command}" >> "${TEMP_FILE}"
                fi
        done < <(ps -e -o comm= | sort | uniq)
done

half_lookups=$((total_lookups/2))

sort -o "${TEMP_FILE}"  

SORTED_TEMP=$(mktemp)

cp "${TEMP_FILE}"  "${SORTED_TEMP}" 

sort -o -u "${SORTED_TEMP}"

while read line; do
        linesCount=$(grep "${line}" "${TEMP_FILE}" | wc -l)
        if [["${linesCount}" -gt "${half_lookups}"]] ; then
                echo "${line}"
        fi
done < "${SORTED_TEMP}"

rm -f "${TEMP_FILE}"
rm -f "${SORTED_TEMP}"
