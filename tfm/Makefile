CFLAGS += -Wall -W -Wshadow -I./ -O3 -funroll-loops -fomit-frame-pointer -DEXCLUDE_TFM_SQR

all:libtfm.a

OBJECTS = \
fp_set.o \
\
fp_rshd.o fp_lshd.o fp_div_2d.o fp_mod_2d.o fp_mul_2d.o fp_2expt.o \
fp_mul_2.o fp_div_2.o  \
\
fp_cnt_lsb.o \
\
fp_add.o fp_sub.o fp_mul.o fp_sqr.o fp_div.o fp_mod.o \
s_fp_add.o s_fp_sub.o \
\
fp_cmp_d.o fp_add_d.o fp_sub_d.o fp_mul_d.o fp_div_d.o fp_mod_d.o \
fp_addmod.o fp_submod.o fp_mulmod.o fp_sqrmod.o fp_invmod.o \
fp_gcd.o fp_lcm.o fp_prime_miller_rabin.o fp_isprime.o \
fp_prime_random_ex.o fp_mul_comba.o fp_sqr_comba.o \
\
fp_montgomery_setup.o fp_montgomery_calc_normalization.o fp_montgomery_reduce.o \
\
fp_exptmod.o \
\
fp_cmp.o fp_cmp_mag.o \
\
fp_unsigned_bin_size.o fp_read_unsigned_bin.o fp_to_unsigned_bin.o \
fp_signed_bin_size.o fp_read_signed_bin.o fp_to_signed_bin.o \
fp_read_radix.o fp_toradix.o fp_radix_size.o fp_count_bits.o fp_reverse.o fp_s_rmap.o \
\
fp_ident.o

libtfm.a: $(OBJECTS)
	ar rcu $@ $+
	ranlib $@

clean:
	rm *.o
	rm *.a
