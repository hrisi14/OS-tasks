#!/bin/bash

if [[ "$()" != "root" ]] ; then
        echo "A non-root user can not do anything with this script!"
        exit 0
fi

TMP_USERS=$(mktemp)

while read current_user; do
        user_dir="~${user}"   #?!

        if [[ ! -e "${user_dir}" ]] ; then
                echo "${user}" >> "${TMP_USERS}"
    fi

        if [[ "$(stat -c '%U' "${user_dir}")" != "${user}" ]] ; then
                echo "${user}" >> "${TMP_USERS}"
        fi

        if [[ ! "$(stat -c '%a' "${user_dir}")"&2 ]] ; then
                echo "${user}" >> "${TMP_USERS}"
        fi
done < <(ps aux | grep -E -v "^root.*" | cut -d ' ' -f 1 | uniq)  


root_total_rss="$(ps -u "root" -o rss= | awk '{sum += $1} END {print sum}')"
uniq "${TMP_USERS}" "${TMP_USERS}"

while read user_line ; do
        total_rss="$(ps -u "${user_line}" -o rss= | awk '{sum += $1} END {print sum}')"
        if [[ "${total_rss}" -gt "${root_total_rss}" ]] ; then
                to_kill="$(ps -u "${user_line}" -o pid= --sort rss)"
                kill "${to_kill}"
                sleep 2
                kill -KILL "${to_kill}"
        fi
done < "${TMP_USERS}"

rm -f "${TMP_USERS}"
