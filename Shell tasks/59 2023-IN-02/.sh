
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
        echo "Error! Wrong arg count!"
        exit 1
fi

if [[ ! -e "${1}" ]] ; then
        echo "Such a directory does NOT exist!"
        exit 1
fi

if [[ ! -d "${1}" ]] ; then
        echo "Error! Arg must be a directory!"
        exit 1
fi


TMP_SHA="$(mktemp)"
TMP_LINKS="$(mktemp)"


echo "$(find "${1}" -type f | xargs -I "X" sha256sum "X" | tr -s ' ' | sort )" >> "${TMP_SHA}"

while read line; do
        file="$(echo "${line}" | cut -d ' ' -f 2)"
        hardlinks="$(stat -c '%h' "${file}" )"
        hashSum="$(echo "${line}" | cut -d ' ' -f 1)"



    if [[ "${hardlinks}" -eq 1 ]] ; then  #just an ordinary file

                if [[ "$(grep -E -c "^${hashSum} .*" "${file}" )" -gt 1 ]] ; then
                        while read relevantFile ; do
                                toRemove="$(echo "${relevantFile}" | cut -d ' ' -f 2)"
                                if [[ "${toRemove}" != "${file}" ]] ; then
                                        echo "${toRemove}"  #rm "${toRemove}"
                            fi
                    done < <(grep -E "^${hashSum} .*"  "${TMP_SHA}")
                fi
        elif [[ "${hardlinks}" -gt 1 ]] ; then
                while read fileLine ; do    #to add a check that ensures this operation is performed only when that grep returns more than one line

                        toRemove="$(echo "${fileLine}" | cut -d ' ' -f 2)"
                        echo "${toRemove}"  #rm "${toRemove}"

                        inode="$(stat -c '%i' "${toRemove}")"
                        removeFromGroup="$(find "${1}" -inum "${inode}" | head -n 1)"
                        if [[ -z "$(grep -qE "${TMP_LINKS}") " ]] ; then
                                echo "${removeFromGroup}" #rm this one
                            echo "${removeFromGroup}" >> "${TMP_LINKS}"
                        fi
                done < <(grep -E "^"${hashSum}" .*")
        fi
done < "${TMP_SHA}"

rm -f "${TMP_SHA}"
rm -f "${TMP_LINKS}"
