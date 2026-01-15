#ifndef MIFARE_TAG_H
#define MIFARE_TAG_H

#include "esp_system.h"

typedef struct {
    uint8_t data[16];
} mifare_block_t;

typedef struct {
    uint8_t keyA[6];
    uint8_t access_bits[3];
    uint8_t user_byte;
    uint8_t keyB[6];
} mifare_sector_trailer_t;

typedef struct {
    mifare_block_t block0;
    mifare_block_t block1;
    mifare_block_t block2;
    mifare_sector_trailer_t trailer;
} mifare_sector_t;


#define MIFARE_SECTOR_COUNT 16

typedef struct {
    mifare_sector_t sectors[MIFARE_SECTOR_COUNT];
} mifare_classic_1k_t;




#endif