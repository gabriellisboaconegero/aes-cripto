#! /usr/bin/bash
# set -x

mkdir -p testes/files/
mkdir -p testes/openssl/
mkdir -p testes/ours/

for i in `seq 20`; do
    kbBytes="$((i * 100))"
    inputFile="testes/files/teste_$(printf '%05d' $kbBytes)_KB"
    openssl rand -out "$inputFile" $((kbBytes*2**10))
done
