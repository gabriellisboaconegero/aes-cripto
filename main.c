#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <openssl/aes.h>
#include "aes.h"

#ifdef TIME
double subBytesTime = 0;
double shiftRowsTime = 0;
double mixColumnsTime = 0;
double addRoundKeyTime = 0;
double totalTime = 0;
#endif

void usage(int argc, char **argv) {
    (void)(argc);
    fprintf(stderr, "Criptografar: \n");
    fprintf(stderr, "%s -e <128_key_string> <plaintext_file> <cipher_file>\n", argv[0]);
    fprintf(stderr, "Decriptografar: \n");
    fprintf(stderr, "%s -d <128_key_string> <cipher_file> <plaintext_file>\n", argv[0]);
    fprintf(stderr, "\t128 bits/16 bytes key. Exemplo '123456789abcdef0'\n");
    fprintf(stderr, "Criptografar com openssl: \n");
    fprintf(stderr, "%s -se <128_key_string> <plaintext_file> <cipher_file>\n", argv[0]);
    fprintf(stderr, "Decriptografar com openssl: \n");
    fprintf(stderr, "%s -sd <128_key_string> <cipher_file> <plaintext_file>\n", argv[0]);
}

int main(int argc, char **argv) {
    BYTE inputKey[30];
    WORD key_schedule[60];
    BYTE outBuf[AES_BLOCK_SIZE*2];
    BYTE inBuf[AES_BLOCK_SIZE*2];
    void (*aes_func)(const BYTE in[], BYTE out[], const WORD key[], int keysize);
    int to_crypt = -1;

    // ============= Entrada =============
    if (argc < 5) {
        usage(argc, argv);
        exit(1);
    }

    if (strcmp(argv[1], "-e") == 0) {
        to_crypt = LOCAL_AES_ENCRYPT;
        aes_func = aes_encrypt;
    }
    else if (strcmp(argv[1], "-d") == 0) {
        to_crypt = LOCAL_AES_DECRYPT;
        aes_func = aes_decrypt;
    }
    else if (strcmp(argv[1], "-se") == 0) {
        to_crypt = REAL_AES_ENCRYPT;
    }
    else if (strcmp(argv[1], "-sd") == 0) {
        to_crypt = REAL_AES_DECRYPT;
    }

    if (to_crypt < 0) {
        fprintf(stderr, "[ERRO]: Parametro '%s' desconhecido\n", argv[1]);
        usage(argc, argv);
        exit(1);
    }

    // 2^7:128 bits / 2^3:8 bits = 2^4:16 bytes
    if (strlen(argv[2]) != 16) {
        fprintf(stderr, "[ERRO]: Chave deve ter 16 bytes (128 bits) de tamnho. Chave '%s' tem %ld\n", argv[2], strlen(argv[2]));
        exit(1);
    }

    FILE *in_f = fopen(argv[3], "r");
    if (in_f == NULL) {
        fprintf(stderr, "[ERRO]: Não foi possível abrir arquivo\n");
        fprintf(stderr, "[ERRNO]: %s\n", strerror(errno));
    }

    FILE *out_f = fopen(argv[4], "w+");
    if (out_f == NULL) {
        fprintf(stderr, "[ERRO]: Não foi possível abrir arquivo\n");
        fprintf(stderr, "[ERRNO]: %s\n", strerror(errno));
    }
    // ============= Entrada =============
    
    // =============  Chave  =============
#ifdef TIME
    if(to_crypt == REAL_AES_ENCRYPT || to_crypt == REAL_AES_DECRYPT)
        totalTime = timestamp();
#endif
    AES_KEY encryptKey, decryptKey;
    memcpy(inputKey, argv[2], 16);
    if(to_crypt == REAL_AES_ENCRYPT)
        AES_set_encrypt_key(inputKey, AES_128, &encryptKey);
    else if(to_crypt == REAL_AES_DECRYPT)
        AES_set_decrypt_key(inputKey, AES_128, &decryptKey);    
    else
        aes_key_setup(inputKey, key_schedule, AES_128);
    // =============  Chave  =============

    // ============= ECB Loop =============
    size_t read_sz = 0;
    size_t total_sz = 0;
    while (!feof(in_f)) {
        read_sz = fread(inBuf, 1, AES_BLOCK_SIZE, in_f);
        if (ferror(in_f) || read_sz == 0)
            break;

        // zero padding
        if (read_sz != AES_BLOCK_SIZE) {
            memset(inBuf+read_sz, '\0', AES_BLOCK_SIZE - read_sz);
        }
#ifdef DEBUG2
        fprintf(stderr, "Lendo bloco 16 bytes: %.*s\r\n", (int)read_sz, inBuf);
#endif
        total_sz += read_sz;
        if (to_crypt == REAL_AES_ENCRYPT)
            AES_encrypt(inBuf, outBuf, &encryptKey);
        else if (to_crypt == REAL_AES_DECRYPT)
            AES_decrypt(inBuf, outBuf, &decryptKey);
        else
            aes_func(inBuf, outBuf, key_schedule, AES_128);
        fwrite(outBuf, 1, AES_BLOCK_SIZE, out_f);
    }
    if (ferror(in_f)) {
        fprintf(stderr, "[ERRO]: Houve um erro ao ler o arquivo '%s'. Abortando\n", argv[3]);
        exit(1);
    }

    // Adiciona 16 bytes, onde o 1 bytes guarda quantos bytes foram lidos no ultimo bloco
    if (to_crypt == LOCAL_AES_ENCRYPT) {
        memset(outBuf, '\0', AES_BLOCK_SIZE);
        outBuf[0] = (BYTE)(AES_BLOCK_SIZE - read_sz) % AES_BLOCK_SIZE;
        fwrite(outBuf, 1, AES_BLOCK_SIZE, out_f);
    } else if (to_crypt == LOCAL_AES_DECRYPT) {
        long new_size = total_sz - (AES_BLOCK_SIZE + (long)inBuf[0]);
#ifdef DEBUG
        fprintf(stderr, "Ultimo bloco tem %d a mais bytes\n", AES_BLOCK_SIZE + (int)inBuf[0]);
        fprintf(stderr, "Diminuindo para %ld\n", new_size);
#endif
        fseek(out_f, new_size, SEEK_SET);
        if (ftruncate(fileno(out_f), ftell(out_f)) != 0) {
            perror("Erro ao truncar o arquivo");
            fclose(out_f);
            fclose(in_f);
            exit(1);
        }
    }

#ifdef TIME
    if(to_crypt != REAL_AES_ENCRYPT && to_crypt != REAL_AES_DECRYPT)
    {
        printf("SubBytesTime: %fms\n", subBytesTime);
        printf("ShiftRowsTime: %fms\n", shiftRowsTime);
        printf("MixColumnsTime: %fms\n", mixColumnsTime);
        printf("AddRoundKeyTime: %fms\n", addRoundKeyTime);
        printf("totalTime: %fms\n", subBytesTime + shiftRowsTime + mixColumnsTime + addRoundKeyTime);
    }
    else
        totalTime = timestamp() - totalTime;
    printf("realTotalTime: %f ms\n", totalTime);
#endif

#ifdef DEBUG
    fprintf(stderr, "total_lido: %ld\n", total_sz);
    fprintf(stderr, "last_read: %ld\n", read_sz);
    if (read_sz == 0) {
        fprintf(stderr, "Arquivo com tamanho multiplo de %d\n", AES_BLOCK_SIZE);
    } else {
        fprintf(stderr, "Arquivo com tamanho não multiplo de %d\n", AES_BLOCK_SIZE);
    }
#endif
    fclose(out_f);
    fclose(in_f);
    // ============= ECB Loop =============
}
