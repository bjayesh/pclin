/*
 * Copyright (c) 2018 Itron Inc.
 * All rights reserved.
 */

#include <tfm.h>
#include <stdint.h>
#include "pc.h"
 
/*
 * global constants - curve secp256r1
 *
 * p = 0xFFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF
 * a = 0xFFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFC
 * b = 0x5AC635D8 AA3A93E7 B3EBBD55 769886BC 651D06B0 CC53B0F6 3BCE3C3E 27D2604B
 *
 */

char *prime256_p_str = 
            "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF";
char *prime256_a_str = 
            "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC";
char *prime256_b_str = 
            "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B";
fp_int param_p, param_a, param_b;

#define IN
#define OUT
#define INOUT

#define TRUE    1
#define FALSE   0

/* error codes */
#define ERR_NO_SQRT                 200
#define ERR_OSTR_TOO_SMALL          201
#define EC_POINT_PRIME256v1_X_SIZE  32

//globals
 static fp_int three, u, y, z;

/***************************************************************************//**
  \details: b = sqrt(g) mod p
 
  \return   int
 
  \retval   FP_OKAY  - success
  \retval   ERR_NO_SQRT - no square root exists for g
*******************************************************************************/
static int 
sqrt_mod_p(
    fp_int *g, 
    fp_int *p, 
    fp_int *b
) 
{
    /* ANSI X9.62-2005 l1.4, page 85 Alg 1 */
    /* curve secp256r1 */

    int ret;

    if (fp_iszero(g) == FP_YES){
        fp_zero(b);
        return FP_OKAY;
    }

    /* u = (p -3)/4 */
    fp_set(&three, 3); 
    fp_sub(p, &three,&u);
    fp_div_2(&u, &u);
    fp_div_2(&u, &u);

    /* y = g^(u+1) mod p */
    fp_add_d(&u,1,&u);
    ret = fp_exptmod(g,&u,p,&y);
    if (ret != FP_OKAY) {
        printf("fp_exptmod fail %d\n", ret);
        return ret;
    }

    /* z = y^2 mod p */
    ret = fp_sqrmod(&y, p, &z);
    if (ret != FP_OKAY) {
        printf("fp_sqrmod fail %d\n", ret);
        return ret;
    } 

    ret = fp_cmp(g, &z);
    if (ret == FP_EQ){
        ret = FP_OKAY;
        fp_copy(&y,b); /* b = y */
    }   
    else
        ret = ERR_NO_SQRT;

    return ret;
}

/***************************************************************************//**
  \details: populate octet string from bignum
 
  \return   int
 
  \retval   0  - success
  \retval   ERR_OSTR_TOO_SMALL - no square root exists for g
*******************************************************************************/
static int
bigint_to_octetstring(
    IN fp_int *bi, 
    OUT uint8_t *ostr, 
    INOUT uint16_t *ostr_len
)
{
    int len = fp_unsigned_bin_size(bi);

    if (len > *ostr_len) return ERR_OSTR_TOO_SMALL;
    fp_to_unsigned_bin(bi, ostr);
    *ostr_len = len;

    return 0;
}

/***************************************************************************//**
  \details: Convert a compressed asymmetric key to uncompressed.
 
  \return   bool
 
  \retval   true  - success
  \retval   false - failed
*******************************************************************************/
bool SSL_ItronEnhanced_ExpandKey(
  uint8_t *inptr,     ///< [in]  Ptr to compressed key to expand.  Expects 33 bytes.
  uint8_t *outptr     ///< [out] Ptr to buffer to store uncompressed key.  Must be >= 65 bytes.
  )
{
    /* ANSI X9.62-2005 A3.1.3.2 */
    int ret;
    uint8_t pc,zp;
    uint8_t *xp , yp[EC_POINT_PRIME256v1_X_SIZE];
    fp_int xpi, three, alpha, beta, tmp1,ypi;

    pc = inptr[0];
    if (pc != 0x02 && pc != 0x03) return FALSE;

    //XPi = octetstring_to_bigint(XP);
    xp = inptr + 1;
    fp_read_unsigned_bin(&xpi,xp, EC_POINT_PRIME256v1_X_SIZE);

    /* prepare p, a and b for supported curve */
    ret = fp_read_radix(&param_p, prime256_p_str, 16);
    if (ret != FP_OKAY) {
        printf("Expandkey error - fp_read_radix param_p\n");
        return FALSE;
    }
    ret = fp_read_radix(&param_a, prime256_a_str, 16);
    if (ret != FP_OKAY) {
        printf("Expandkey error - fp_read_radix param_a\n");
        return FALSE;
    }
    ret = fp_read_radix(&param_b, prime256_b_str, 16);
    if (ret != FP_OKAY) {
        printf("Expandkey error - fp_read_radix param_b\n");
        return FALSE;
    }

    if (pc == 0x02)
        zp = 0;
    if (pc == 0x03)
        zp = 1;

    /* derive YPi */
    /* alpha = (XPi*XPi*XPi + a*XPi + b) mod  p; */
    fp_set(&three, 3); 
    ret = fp_exptmod(&xpi, &three, &param_p, &alpha);
    if (ret != FP_OKAY){
        printf("Expandkey error - fp_exptmod\n");
        return FALSE;
    }
    /* a*XPi */
    ret = fp_mulmod(&param_a, &xpi, &param_p, &tmp1);
    if (ret != FP_OKAY){
        printf("Expandkey error - fp_mulmod\n");
        return FALSE;
    }
    ret = fp_addmod(&alpha, &tmp1, &param_p, &alpha);
    if (ret != FP_OKAY){
        printf("Expandkey error - fp_addmod\n");
        return FALSE;
    }
    ret = fp_addmod(&alpha, &param_b, &param_p, &alpha);
    if (ret != FP_OKAY){
        printf("Expandkey error - fp_addmod\n");
        return FALSE;
    }

    /* beta = sqrt_mod_p(alpha) */
    ret = sqrt_mod_p(&alpha, &param_p, &beta);
    if (ret != FP_OKAY) {
        printf("Expandkey error - sqrt_mod_P err %d \n", ret);
        return FALSE;
    } 
    /* if (LSBit(beta) == Zp) */
    if ( (beta.dp[0] & 1) == zp ){
        fp_copy(&beta,&ypi);
    }
    else {
        fp_sub(&param_p, &beta, &ypi);
    }

    uint16_t ostr_len = 32;
    ret = bigint_to_octetstring(&ypi, yp, &ostr_len);
    if (ret != 0 ) {
        printf("Expandkey error - bigint_to_ocetstring err %d \n", ret);
        return FALSE;
    }

    /* prepare output */
    outptr[0] = 0x04;
    memcpy(outptr + 1, xp, EC_POINT_PRIME256v1_X_SIZE);
    memcpy(outptr + 1 + EC_POINT_PRIME256v1_X_SIZE, yp, EC_POINT_PRIME256v1_X_SIZE);

    return TRUE;
}

/***************************************************************************//**
  \details  Compress an asymmetric public key.  Expects 65 bytes starting with 0x04.
 
  \return   bool
 
  \retval   true  - success
  \retval   false - failed
*******************************************************************************/
bool SSL_ItronEnhanced_CompressKey(
  uint8_t *pubKey,          ///< [in]  Only public key can be compressed.
  uint8_t *compressedKey    ///< [out] Must hold 33 bytes.
  )
{
    /* ANSI X9.62-2005 A5.7 */

    uint8_t pc,*yp;
    fp_int ypi;

    pc = pubKey[0];
    if (pc != 0x04) return FALSE;

    /* covert yp to bignum ypi */
    yp = pubKey + EC_POINT_PRIME256v1_X_SIZE + 1;
    fp_read_unsigned_bin(&ypi,yp, EC_POINT_PRIME256v1_X_SIZE);

    /* derive PC of compressed point */
    fp_mod_2d(&ypi, 1, &ypi);
    if (fp_cmp_d(&ypi,1) == FP_EQ)
        pc = 0x03;
    else
        pc = 0x02;

    /* prepare output */
    compressedKey[0] = pc;
    memcpy(compressedKey + 1, pubKey + 1,EC_POINT_PRIME256v1_X_SIZE);

    return TRUE;
}
