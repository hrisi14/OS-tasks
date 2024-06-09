
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
        echo "Error! Wrong arguments count!"
        exit 1
fi

if [[ ! -e "${1}" ]] ; then
        echo "Error! File does not exist!"
        exit 1
fi

if [[ !-r "${1}" ]] ; then
        echo "Error! File does not have read rights!"
        exit 1
fi

#Do I need to do this check???
TMP_CHECK=$(mktemp)
echo "$(grep -E '.*\/HTTP\/.*'  "${1}" | cut -d ' ' -f 8 | cut -d '/' -f 2)" >> "${TMP_CHECK}"
while read line ; do
        if [[ "${line}" != "1.0" && "${line}" != "1.1" && "${line}" != "2.0" ]] ; then
                echo "Error! Invalid HTTP protocol!"
                exit 1
        fi
done < "${TMP_CHECK}"
#End of questionable check

rm -f "${TMP_CHECK}"

TMP_SITES=$(mktemp)
TMP_CLIENTS=$(mktemp)

echo "$(cat "${1}" | cut -d ' ' -f 2 | sort | uniq)" > "${TMP_SITES}"  2>>/dev/null
echo "$(cat "${1}" | cut -d ' ' -f 1 | sort| uniq)" > "${TMP_CLIENTS}" 2>>/dev/null

while read site; do
        COUNT="$(grep -E -w "${site}" | wc -l)"
        sed -E -i "s/^(${site})$/${COUNT} \1/g"
done < "${TMP_SITES}"

echo "$(sort -t ' ' -k 1 "${TMP_SITES}" | head -n 3)" > "${TMP_SITES}"


while read site ; do
        PART_SITE=$(mktemp)
        CLIENTS_TPM=$(mktemp)

        HTTP_COUNT=$(grep -E -w "${site}"  "${1}" | grep -E '.*\/ HTTP\/[12]\.[01]' | wc -l)

        NON_HTTP_COUNT=$(grep -E -w "${site}" "${1}" | grep -E '\/.+ HTTP\/.+' | wc -l)


        echo "$(grep -E -w "${site}" "${1}" | cut -d ' ' -f 1 | sort | uniq)" >> "${CLIENTS_TMP}"

        while read client; do
                REQUEST_CODE="$(grep "^${client} "${site}"" | cut -d ' ' -f 9)"
                CLIENT_COUNT="$(grep "^${client} ${site}.*$" | wc -l)"
                echo "${client} ${CLIENT_COUNT}" >> "${CLIENTS_TMP}"
        done < "${CLIENTS_TMP}"

        sort -o -n -t ' ' -k 2  "${CLIENTS_TMP}" "${CLIENTS_TMP}"
        rm -f "${PART_SITE}"

        echo "${site} HTTP: ${HTTP_COUNT} non-HTTP: ${NON_HTTP_COUNT}"
        echo "$("${CLIENTS_TMP}" | head -n 3)"

    rm -f "${CLIENTS_TMP}"
done "${TMP_SITES}"

