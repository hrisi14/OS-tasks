if [[ "${#}" -ne 3  ]] ; then
    echo "Error! Wrong arg count!"
exit 1
fi

line1=$(grep '"${2}"=' "${1}")
regExpression=$(echo "${line1}" | sed 's/ /|/g' )
line2=$(grep '"${3}"=' "${1}")

newValue=$(echo "${line2}" | sed 's/"${regExpression}"/ /g') 

newLine='"${3}"="${newValue}"'

sed 's/'"${3}"=*/"${newLine}"' "${1}"
