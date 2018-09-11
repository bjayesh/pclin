/*
 * Copyright (c) 2018 Itron Inc.
 * All rights reserved.
 */

#ifndef _PC_H_
#define _PC_H_

#include <stdbool.h>

#define IN
#define OUT
#define INOUT

#define TRUE    1
#define FALSE   0

/* error codes */
#define ERR_NO_SQRT                 200
#define ERR_OSTR_TOO_SMALL          201
#define EC_POINT_PRIME256v1_X_SIZE  32

bool SSL_ItronEnhanced_ExpandKey(uint8_t *inptr, uint8_t *outptr);
bool SSL_ItronEnhanced_CompressKey( uint8_t *pubKey, uint8_t *compressedKey);

#endif
