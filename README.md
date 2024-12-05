# Implementação do algoritmo AES (Advanced Encryption Standard)
O objetivo desse relatório é apresentar as escolhas feitas ao implmentar
uma versão modificada do algoritmo AES. A única modificação feita foi
a troca da cifra de substituição, que antes era feita com Caixas-S (S-Box)
e agora é feita com **nossa cifra**.

## Ambiente de teste
Para poder comparar de forma justa com a `openssl` vamos utilizar um docker com
a biblioteca recompilada sem otimizações de compilador e sem usar assembly.

Para isso vamos recompilar a `openssl` com os nossos parametros. Pra isso vamos seguir o
tutorial em https://github.com/openssl/openssl/blob/master/INSTALL.md.
Para clonar o repositório basta
```bash
git clone git@github.com:openssl/openssl.git
```

Visto os pré-requsitos vamos subir um docker com eles e começar a instalação.
>> Usar docker para grantir que não vamos cagar nossa máquina :).
Para começar crie a imagem usando o Dockerfile.
```bash
docker build -t aes-crypto .
```
Sim, vai demorar muito. Afinal está compilando a `openssl` do zero.

Após o processo acesse o docker e faça alguns testes de performace, como compara entre
a `openssl` que já vem no container e a que acabamos de compilar, que está em `/home/openssl-test`.
```bash
docker run aes-crypto --name aes-crypto
```
Para acessar as próximas vezes basta
```bash
docker exec -it aes-crypto /bin/bash
```

Primeiro vamos gerar um arquivo de tamanho consideravelmente grande (1.1 Giga) e criptografar com
`aes-128-ecb`.

```bash
# Gera arquivo de 1.1 Giga
openssl rand -out sample.txt -base64 $(( 2**30 * 3/4 ))
```

Agora podemos comparar
```bash
# Testa com openssl nativa
time openssl enc -aes-128-ecb -K '112233445566778899aabbccddeeff00' -in sample.txt -out cipher_otm.bin
# Testa com openssl compilada manualmente
time openssl-test/usr/local/bin/openssl enc -aes-128-ecb -K '112233445566778899aabbccddeeff00' -in sample.txt -out cipher_comp.bin
```
## Análise de performace
Foi utilizado o tamanho de chave 128 bits e comparada com a versão **escolher pior versão**.

## Tabelas e essas coisas

