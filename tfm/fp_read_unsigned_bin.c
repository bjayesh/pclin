#include <tfm.h>

void fp_read_unsigned_bin(fp_int *a, const unsigned char *b, int c)
{
  /* zero the int */
  fp_zero (a);

  /* read the bytes in */
  for (; c > 0; c--) {
    fp_mul_2d (a, 8, a);
    a->dp[0] |= *b++;
    a->used += 1;
  }
  fp_clamp (a);
}

