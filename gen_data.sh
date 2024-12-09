#! /usr/bin/bash
# set -x
make time || exit 1

# Limpa arquivo
:>data.csv

for inputFile in $(ls testes/files/*); do
    baseFile="$(basename $inputFile)"
    cipherFile_1="testes/openssl/${baseFile}.cipher"
    cipherFile_2="testes/ours/${baseFile}.cipher"
    outFile_1="testes/openssl/${baseFile}.out"
    outFile_2="testes/ours/${baseFile}.out"
    kbBytes="$(echo $baseFile | cut -d'_' -f2)"
    echo -n "$kbBytes," >> data.csv

    echo "OURS: Processando $inputFile"
    encTime_2="$(./aes -e 123456789abcdef0 "$inputFile" "$cipherFile_2" | grep 'realTotalTime' | cut -d' ' -f2)"
    decTime_2="$(./aes -d 123456789abcdef0 "$cipherFile_2" "$outFile_2" | grep 'realTotalTime' | cut -d' ' -f2)"
    if cmp -s "$inputFile" "$outFile_2"; then
        echo -n "$encTime_2,$decTime_2," >> data.csv
    else
        echo -e "\x1b[31mERRO\x1b[m: Descriptografia deu errado"
    fi

    echo "OPENSSL: Processando $inputFile"
    encTime_1="$(./aes -se 123456789abcdef0 "$inputFile" "$cipherFile_1" | grep 'realTotalTime' | cut -d' ' -f2)"
    decTime_1="$(./aes -sd 123456789abcdef0 "$cipherFile_1" "$outFile_1" | grep 'realTotalTime' | cut -d' ' -f2)"
    if cmp -s "$inputFile" "$outFile_1"; then
        echo "$encTime_1,$decTime_1" >> data.csv
    else
        echo -e "\x1b[31mERRO\x1b[m: Descriptografia deu errado"
    fi
done
