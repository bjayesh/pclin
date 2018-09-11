#include <tfm.h>

/* This is possibly the mother of all prime generation functions, muahahahahaha! */
int fp_prime_random_ex(fp_int *a, int t, int size, int flags, tfm_prime_callback cb, void *dat)
{
   unsigned char *tmp, maskAND, maskOR_msb, maskOR_lsb;
   int first, zero, x, res, err, bsize, maskOR_msb_offset;
   fp_digit sieve[256];

   /* sanity check the input */
   if (size <= 1 || t <= 0) {
      return FP_VAL;
   }

   /* TFM_PRIME_SAFE implies TFM_PRIME_BBS */
   if (flags & TFM_PRIME_SAFE) {
      flags |= TFM_PRIME_BBS;
   }

   /* calc the byte size */
   bsize = (size>>3)+(size&7?1:0);

   /* we need a buffer of bsize bytes */
   tmp = malloc(bsize);
   if (tmp == NULL) {
      return FP_MEM;
   }

   /* calc the maskAND value for the MSbyte*/
   maskAND = 0xFF >> (8 - (size & 7));

   /* calc the maskOR_msb */
   maskOR_msb        = 0;
   maskOR_msb_offset = (size - 2) >> 3;
   if (flags & TFM_PRIME_2MSB_ON) {
      maskOR_msb     |= 1 << ((size - 2) & 7);
   } else if (flags & TFM_PRIME_2MSB_OFF) {
      maskAND        &= ~(1 << ((size - 2) & 7));
   }

   /* get the maskOR_lsb */
   maskOR_lsb         = 1;
   if (flags & TFM_PRIME_BBS) {
      maskOR_lsb     |= 3;
   }

   first = 1;
   do {

      if (first == 1) {
         /* read the bytes */
         if (cb(tmp, bsize, dat) != bsize) {
            err = FP_VAL;
            goto error;
         }
 
         /* work over the MSbyte */
         tmp[0]    &= maskAND;
         tmp[0]    |= 1 << ((size - 1) & 7);

         /* mix in the maskORs */
         tmp[maskOR_msb_offset]   |= maskOR_msb;
         tmp[bsize-1]             |= maskOR_lsb;

         /* read it in */
         fp_read_unsigned_bin(a, tmp, bsize);

         /* generate sieve table */
         for (x = 1; x < 256; x++) { fp_div_d(a, tfm_primes[x], NULL, &sieve[x]); }
      } else {
         do  {
             /* add 2 */
             fp_add_d(a, 2, a);

             /* add 2 to sieve table */
             zero = 0;
             for (x = 1; x < 256; x++) { sieve[x] += 2; if (sieve[x] >= tfm_primes[x]) sieve[x] -= tfm_primes[x]; if (sieve[x] == 0) zero = 1; }
         } while (zero == 1);
      } 

      /* is it prime? */
      res = fp_isprime(a);
      if (res == FP_NO) continue;

      if (flags & TFM_PRIME_SAFE) {
         /* see if (a-1)/2 is prime */
         fp_sub_d(a, 1, a);
         fp_div_2(a, a);
 
         /* is it prime? */
         res = fp_isprime(a);
      }
   } while (res == FP_NO);

   if (flags & TFM_PRIME_SAFE) {
      /* restore a to the original value */
      fp_mul_2(a, a);
      fp_add_d(a, 1, a);
   }

   err = FP_OKAY;
error:
   free(tmp);
   return err;
}

