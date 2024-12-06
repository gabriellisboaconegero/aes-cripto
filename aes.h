/*********************************************************************
* Filename:   aes.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding AES implementation.
*********************************************************************/

#ifndef _AES_H
#define _AES_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define AES_BLOCK_SIZE 16               // AES operates on 16 bytes at a time
#define AES_128 128
#define AES_ENCRYPT 1
#define AES_DECRYPT 2

/***************************** EXTERNS ******************************/
extern double subBytesTime;
extern double shiftRowsTime;
extern double mixColumnsTime;
extern double addRoundKeyTime;
extern double totalTime;

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;            // 8-bit byte
typedef unsigned int WORD;             // 32-bit word, change to "long" for 16-bit machines

/*********************** FUNCTION DECLARATIONS **********************/
///////////////////
// AES
///////////////////
// Key setup must be done before any AES en/de-cryption functions can be used.
void aes_key_setup(const BYTE key[],          // The key, must be 128, 192, or 256 bits
                   WORD w[],                  // Output key schedule to be used later
                   int keysize);              // Bit length of the key, 128, 192, or 256

void aes_encrypt(const BYTE in[],             // 16 bytes of plaintext
                 BYTE out[],                  // 16 bytes of ciphertext
                 const WORD key[],            // From the key setup
                 int keysize);                // Bit length of the key, 128, 192, or 256

void aes_decrypt(const BYTE in[],             // 16 bytes of ciphertext
                 BYTE out[],                  // 16 bytes of plaintext
                 const WORD key[],            // From the key setup
                 int keysize);                // Bit length of the key, 128, 192, or 256

///////////////////
// Test functions
///////////////////
int aes_test();
int aes_ecb_test();

///////////////////
// Time functions
///////////////////
double timestamp(void);

#endif   // AES_H
