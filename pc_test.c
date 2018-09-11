#include <tfm.h>
#include <stdint.h>
#include "pc.h"

#define TRUE  1
#define FALSE 0
#define EC_P256_PUBKEY_SIZE  65

char *ecpt_strings[] = {
    "04"
    "6099bdd1f5582c2ff4f4fbe2315d93179028e349f0bad43fc9c65549e83a5c62"
    "661dda2413439903bd2069975629e77f2107811d7ceb51dbc2d96852c4f0b293",

    "04"
    "a952e847a5e118b45ba3b6c14682d2d067db36a9d4060fe7a2f230e01e028ced"
    "9d0ad7f74d24102708cce38932e2bfb88dea945fe33e9b9e0f4c0f0882cddf09",
    
    "04"
    "f840271919c420c2e3cd3f53fa3a142dc5fd683f3d0db074802ce7f3ac5eb926"
    "77ec91cca39d0ca6a9e36437abdcdcd844ae0bc6596caaa4b10775687cc33ff8",
    
    "04"
    "7616813a02bec8d52df2f8601a722fa72fe51aee7c2494294ba013ac2b012ef0"
    "c2332bbde0cfc847478aaa4e1839dabd5c34762191409946418dd004c9013b2e",

    "04"
    "2afa1ff6df9850319e42ac4768f0d3f177299c7f409b9103c1e08a459fdeea56"
    "e4e45af1d153c854d7fdf51f31cbd94c92c9faf46e5abca68d51fd99a6b5f9ba",

    "04"
    "e26960390c5f860fb3731e82c7d8ef16e8dccbd6a583cdafa518c7c101c82ead"
    "dca9dfd4cc07f46647159b940e4f03e89869b1b1eb5e6133011d9193dce52019",

    "04"
    "efae29804bff011dde29cde3f0c0106813a3413e7dce3864b9b755c32c419090"
    "6b68c09ccc4acb2210e83b0214e8c696eaa4ac003c9e638dd7f4ce207bd52311",

    "04"
    "e05788653983d48a38ccc3f701cf239b0f8b5b85ac05d2afb5172269678b7e95"
    "d2c37c78df75cf32d5ba8ff0d91a4e777c14e3494729e771eb716c27681093f8",

    "04"
    "1445a8da1d8103d194093b93c4a3458382c7643923c81ec591cde9031beb65e9"
    "c86363c896ec7114a479afe7413d379dfcee2ebc18cd7f06fe696c358fa00a0e",

    "04"
    "4f4f9603e10f904c60f6f9bbac6c1c9c075820325a8dc712738cb32eade87b9b"
    "5363c47458ab33277be77af342c8c5ad97f1d1e63832887ea1a365435b61b212",

};

uint8_t ecpt_uc_1[EC_P256_PUBKEY_SIZE];
uint8_t ecpt_uc_2[EC_P256_PUBKEY_SIZE];
uint8_t ecpt_c[EC_P256_PUBKEY_SIZE];

#define ERR_BUF_LENGTH 201
#define ERR_NO_PAD     202

void print_bn(char *msg, fp_int *bn)
{
    int l,i;
    uint8_t print_string[64];
 
    l = fp_unsigned_bin_size(bn);
    fp_to_unsigned_bin(bn,print_string);

    printf("%s :", msg); 
    printf("[%d octets] :", l);
    for (i = 0; i < l; i++)
        printf("%02x ", print_string[i]);
    printf("\n");
}

uint8_t get_digit(char c)
{
    if (c >= '0' && c <= '9') c -= '0';
    if (c >= 'a' && c <= 'f') c = c + 10 - 'a';
    if (c >= 'A' && c <= 'F') c = c + 10 - 'A';

    return c;
}

int ecpt_str_to_ostr(char *str, uint8_t *ostr,uint16_t *ostr_len)
{
    uint16_t slen = strlen(str);
    uint8_t cursor, i;

    if (slen %2 ) return ERR_NO_PAD;
    if (slen > (EC_P256_PUBKEY_SIZE * 2) || *ostr_len < 33) return ERR_BUF_LENGTH;

    cursor = 0;
    *ostr_len = slen/2;

    for (i = 0; i < *ostr_len; i++){
        ostr[i] = get_digit(str[cursor])*16 + get_digit(str[cursor+1]);
        cursor+=2;
    }

    return 0;
}

void print_ostr(char *msg, uint8_t *ostr,uint16_t len)
{
    uint16_t i;

    printf("%s :", msg);
    for (i = 0; i < len; i++)
        printf("%02x", ostr[i]);
    printf("\n");
}

int main()
{
    uint16_t ostr_len;
    int i;
    bool ret;
    uint8_t no_of_points = sizeof(ecpt_strings)/sizeof(char *);

    printf("EC point compress/uncompress test\n");
    for (i = 0; i < no_of_points; i++) {

        //convert string to ostring
        ostr_len = EC_P256_PUBKEY_SIZE;
        ecpt_str_to_ostr(ecpt_strings[i], ecpt_uc_1, &ostr_len);

        // call compress
        ret = SSL_ItronEnhanced_CompressKey(ecpt_uc_1, ecpt_c); 
        if (ret == FALSE) {
            printf("Test -- %02d Error point compress \n", i);
            exit(1);
        }

        // call uncompress
        ret = SSL_ItronEnhanced_ExpandKey(ecpt_c, ecpt_uc_2); 
        if (ret == FALSE) {
            printf("Test -- %02d Error point expand \n", i);
            exit(1);
        }

        //check results
        if (memcmp(ecpt_uc_2, ecpt_uc_1, EC_P256_PUBKEY_SIZE) == 0){
            printf("Test %02d -- PASS.\n", i+1);
        }
        else {
            printf("Test %02d -- FAIL.\n", i+1);
        }
    }
    return 0;
}
