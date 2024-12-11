# Alunos
- Gabriel Lisboa Conegero: GRR20221255
- Daniel Celestino de Lins Neto: GRR20220064

# Implementação do algoritmo AES (Advanced Encryption Standard)
O objetivo desse relatório é apresentar as escolhas feitas ao implmentar
uma versão modificada do algoritmo AES. A única modificação feita foi
a troca da cifra de substituição, que antes era feita com Caixas-S (S-Box)
e agora é feita com **nossa cifra**. Nossa cifra é um **XOR** com os bytes
da matriz.

## Padding
O padding é feito colocando 16 bytes a mais no final do arquivo criptografado
onde o primeiro byte contém a quantidade de bytes que foi adicionado no padding
do bloco anterior.

## Testes
Foi usada a API da openssl para criptografar uma mensagem e comparar com a nossa cifra.

Gráfics foram gerados para apresentar o desempenho da nossa cifra.
```shell
./gen_test_files.sh
./gen_data.sh
./gen_graphs.sh
```
