/*-----------------------------------------------------------------------
//
// Proprietary Information of Elliptic Semiconductor
// Copyright (C) 2002-2007, all rights reserved
// Elliptic Semiconductor, Inc.
//
// As part of our confidentiality  agreement, Elliptic Semiconductor and
// the Company, as  a  Receiving Party, of  this  information  agrees to
// keep strictly  confidential  all Proprietary Information  so received
// from Elliptic Semiconductor. Such Proprietary Information can be used
// solely for  the  purpose  of evaluating  and/or conducting a proposed
// business  relationship  or  transaction  between  the  parties.  Each
// Party  agrees  that  any  and  all  Proprietary  Information  is  and
// shall remain confidential and the property of Elliptic Semiconductor.
// The  Company  may  not  use  any of  the  Proprietary  Information of
// Elliptic Semiconductor for any purpose other  than  the  above-stated
// purpose  without the prior written consent of Elliptic Semiconductor.
//
*/

#include <tfm.h>

/* b = a*a  */
void fp_sqr(fp_int *A, fp_int *B)
{
    int     y;

    /* call generic if we're out of range */
    if (A->used + A->used > FP_SIZE) {
       fp_sqr_comba(A, B);
       return ;
    }

    y = A->used;
#if defined(TFM_SQR3)
        if (y <= 3) {
           fp_sqr_comba3(A,B);
           return;
        }
#endif
#if defined(TFM_SQR4)
        if (y == 4) {
           fp_sqr_comba4(A,B);
           return;
        }
#endif
#if defined(TFM_SQR6)
        if (y <= 6) {
           fp_sqr_comba6(A,B);
           return;
        }
#endif
#if defined(TFM_SQR7)
        if (y == 7) {
           fp_sqr_comba7(A,B);
           return;
        }
#endif
#if defined(TFM_SQR8)
        if (y == 8) {
           fp_sqr_comba8(A,B);
           return;
        }
#endif
#if defined(TFM_SQR9)
        if (y == 9) {
           fp_sqr_comba9(A,B);
           return;
        }
#endif
#if defined(TFM_SQR12)
        if (y <= 12) {
           fp_sqr_comba12(A,B);
           return;
        }
#endif
#if defined(TFM_SQR17)
        if (y <= 17) {
           fp_sqr_comba17(A,B);
           return;
        }
#endif
#if defined(TFM_SMALL_SET)
        if (y <= 16) {
           fp_sqr_comba_small(A,B);
           return;
        }
#endif
#if defined(TFM_SQR20)
        if (y <= 20) {
           fp_sqr_comba20(A,B);
           return;
        }
#endif
#if defined(TFM_SQR24)
        if (y <= 24) {
           fp_sqr_comba24(A,B);
           return;
        }
#endif
#if defined(TFM_SQR28)
        if (y <= 28) {
           fp_sqr_comba28(A,B);
           return;
        }
#endif
#if defined(TFM_SQR32)
        if (y <= 32) {
           fp_sqr_comba32(A,B);
           return;
        }
#endif
#if defined(TFM_SQR48)
        if (y <= 48) {
           fp_sqr_comba48(A,B);
           return;
        }
#endif
#if defined(TFM_SQR64)
        if (y <= 64) {
           fp_sqr_comba64(A,B);
           return;
        }
#endif
       fp_sqr_comba(A, B);
}


