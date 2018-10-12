#include <tommath.h>
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
mp_int param_p, param_a, param_b;

#define IN
#define OUT
#define INOUT

#define TRUE    1
#define FALSE   0

/* error codes */
#define ERR_NO_SQRT                 200
#define ERR_OSTR_TOO_SMALL          201
#define EC_POINT_PRIME256v1_X_SIZE  32

extern void print_bn(char *msg, mp_int *bn);

//globals
static __attribute__((aligned(4))) mp_int three, alpha, beta, u, y, z;
static __attribute__((aligned(4))) mp_int xpi,tmp1,ypi;


/***************************************************************************//**
  \details: b = sqrt(g) mod p
 
  \return   int
 
  \retval   MP_OKAY  - success
  \retval   ERR_NO_SQRT - no square root exists for g
*******************************************************************************/
static int 
sqrt_mod_p(
    mp_int *g, 
    mp_int *p, 
    mp_int *b
) 
{
    /* ANSI X9.62-2005 l1.4, page 85 Alg 1 */
    /* curve secp256r1 */

    int ret;

    if (mp_cmp_d(g, 0) == MP_EQ){
        mp_set(b, 0);
        return MP_OKAY;
    }

    /* u = (p -3)/4 */
    mp_set(&three, 3); 
    mp_sub(p, &three,&u);
    mp_div_2(&u, &u);
    mp_div_2(&u, &u);

    /* y = g^(u+1) mod p */
    mp_add_d(&u,1,&u);
    ret = mp_exptmod(g,&u,p,&y);
    if (ret != MP_OKAY) {
        printf("mp_exptmod fail %d\n", ret);
        return ret;
    }

    /* z = y^2 mod p */
    ret = mp_sqrmod(&y, p, &z);
    if (ret != MP_OKAY) {
        printf("mp_sqrmod fail %d\n", ret);
        return ret;
    } 

    ret = mp_cmp(g, &z);
    if (ret == MP_EQ){
        ret = MP_OKAY;
        mp_copy(&y,b); /* b = y */
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
    IN mp_int *bi, 
    OUT uint8_t *ostr, 
    INOUT uint16_t *ostr_len
)
{
    int len = mp_unsigned_bin_size(bi);

    if (len > *ostr_len) return ERR_OSTR_TOO_SMALL;
    mp_to_unsigned_bin(bi, ostr);
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
    
    mp_init_multi(&param_p, &param_a, &param_b, &three, 
    &alpha, &beta, &u, &y, &z, &xpi, &tmp1, &ypi, NULL);
    
    pc = inptr[0];
    if (pc != 0x02 && pc != 0x03) return FALSE;

    //XPi = octetstring_to_bigint(XP);
    xp = inptr + 1;
    mp_read_unsigned_bin(&xpi,xp, EC_POINT_PRIME256v1_X_SIZE);

    /* prepare p, a and b for supported curve */
    ret = mp_read_radix(&param_p, prime256_p_str, 16);
    if (ret != MP_OKAY) {
        printf("Expandkey error - mp_read_radix param_p\n");
        return FALSE;
    }
    ret = mp_read_radix(&param_a, prime256_a_str, 16);
    if (ret != MP_OKAY) {
        printf("Expandkey error - mp_read_radix param_a\n");
        return FALSE;
    }
    ret = mp_read_radix(&param_b, prime256_b_str, 16);
    if (ret != MP_OKAY) {
        printf("Expandkey error - mp_read_radix param_b\n");
        return FALSE;
    }

    if (pc == 0x02)
        zp = 0;
    if (pc == 0x03)
        zp = 1;

    /* derive YPi */
    /* alpha = (XPi*XPi*XPi + a*XPi + b) mod  p; */
    mp_set(&three, 3); 
    ret = mp_exptmod(&xpi, &three, &param_p, &alpha);
    if (ret != MP_OKAY){
        printf("Expandkey error - mp_exptmod\n");
        return FALSE;
    }
    /* a*XPi */
    ret = mp_mulmod(&param_a, &xpi, &param_p, &tmp1);
    if (ret != MP_OKAY){
        printf("Expandkey error - mp_mulmod\n");
        return FALSE;
    }
    ret = mp_addmod(&alpha, &tmp1, &param_p, &alpha);
    if (ret != MP_OKAY){
        printf("Expandkey error - mp_addmod\n");
        return FALSE;
    }
    ret = mp_addmod(&alpha, &param_b, &param_p, &alpha);
    if (ret != MP_OKAY){
        printf("Expandkey error - mp_addmod\n");
        return FALSE;
    }

    /* beta = sqrt_mod_p(alpha) */
    ret = sqrt_mod_p(&alpha, &param_p, &beta);
    if (ret != MP_OKAY) {
        printf("Expandkey error - sqrt_mod_P err %d \n", ret);
        return FALSE;
    } 
    /* if (LSBit(beta) == Zp) */
    if ( (beta.dp[0] & 1) == zp ){
        mp_copy(&beta,&ypi);
    }
    else {
        mp_sub(&param_p, &beta, &ypi);
    }
    print_bn("bigint string is", &ypi);
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


    mp_clear_multi(&param_p, &param_a, &param_b, &three,
    &alpha, &beta, &u, &y, &z, &xpi, &tmp1, &ypi, NULL);
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
    mp_init(&ypi);
    
    pc = pubKey[0];
    if (pc != 0x04) return FALSE;

    /* covert yp to bignum ypi */
    yp = pubKey + EC_POINT_PRIME256v1_X_SIZE + 1;
    mp_read_unsigned_bin(&ypi,yp, EC_POINT_PRIME256v1_X_SIZE);

    /* derive PC of compressed point */
    mp_mod_2d(&ypi, 1, &ypi);
    if (mp_cmp_d(&ypi,1) == MP_EQ)
        pc = 0x03;
    else
        pc = 0x02;

    /* prepare output */
    compressedKey[0] = pc;
    memcpy(compressedKey + 1, pubKey + 1,EC_POINT_PRIME256v1_X_SIZE);
    
    mp_clear(&ypi);
    return TRUE;
}
