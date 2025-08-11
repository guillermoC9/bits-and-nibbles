/*
    poly1305.c

    Poly1305 MAC functions

    (CC) Creative Commons 2018-2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Adapted by Guillermo Amodeo Ojeda from Andrew Moon's public domain code
          for poly1305-donna on:

          https://github.com/floodyberry/poly1305-donna.


                                --oO0Oo--
*/

#include "poly1305.h"

typedef unsigned int  ul_t;
typedef u64_t  ull_t;

/* ----------------------------------------- */

static void poly1305_blocks(poly1305_t *st, const unsigned char *m, size_t bytes)
{
	ull_t    r0,r1,r2,r3,r4;
	ull_t    s1,s2,s3,s4;
	ull_t    h0,h1,h2,h3,h4;
	ull_t    d0,d1,d2,d3,d4;
	ul_t     c;

    ul_t hibit = (st->final) ? 0 : 1UL << 24;

	r0 = st->r[0];
	r1 = st->r[1];
	r2 = st->r[2];
	r3 = st->r[3];
	r4 = st->r[4];

	s1 = r1 * 5;
	s2 = r2 * 5;
	s3 = r3 * 5;
	s4 = r4 * 5;

	h0 = st->h[0];
	h1 = st->h[1];
	h2 = st->h[2];
	h3 = st->h[3];
	h4 = st->h[4];

	while (bytes >= POLY1305_BLOCK_SIZE)
	{
		/* h += m[i] */
		h0 += (get_le32(m+ 0)     ) & 0x3ffffff;
		h1 += (get_le32(m+ 3) >> 2) & 0x3ffffff;
		h2 += (get_le32(m+ 6) >> 4) & 0x3ffffff;
		h3 += (get_le32(m+ 9) >> 6) & 0x3ffffff;
		h4 += (get_le32(m+12) >> 8) | hibit;

		/* h *= r */
		d0 = ((ull_t) h0 * r0) + ((ull_t) h1 * s4) + ((ull_t) h2 * s3) + ((ull_t) h3 * s2) + ((ull_t) h4 * s1);
		d1 = ((ull_t) h0 * r1) + ((ull_t) h1 * r0) + ((ull_t) h2 * s4) + ((ull_t) h3 * s3) + ((ull_t) h4 * s2);
		d2 = ((ull_t) h0 * r2) + ((ull_t) h1 * r1) + ((ull_t) h2 * r0) + ((ull_t) h3 * s4) + ((ull_t) h4 * s3);
		d3 = ((ull_t) h0 * r3) + ((ull_t) h1 * r2) + ((ull_t) h2 * r1) + ((ull_t) h3 * r0) + ((ull_t) h4 * s4);
		d4 = ((ull_t) h0 * r4) + ((ull_t) h1 * r3) + ((ull_t) h2 * r2) + ((ull_t) h3 * r1) + ((ull_t) h4 * r0);

		/* (partial) h %= p */
		              c = (ul_t)(d0 >> 26); h0 = (ul_t)d0 & 0x3ffffff;
		d1 += c;      c = (ul_t)(d1 >> 26); h1 = (ul_t)d1 & 0x3ffffff;
		d2 += c;      c = (ul_t)(d2 >> 26); h2 = (ul_t)d2 & 0x3ffffff;
		d3 += c;      c = (ul_t)(d3 >> 26); h3 = (ul_t)d3 & 0x3ffffff;
		d4 += c;      c = (ul_t)(d4 >> 26); h4 = (ul_t)d4 & 0x3ffffff;
		h0 += c * 5;  c =                (h0 >> 26); h0 =                h0 & 0x3ffffff;
		h1 += c;

		m += POLY1305_BLOCK_SIZE;
		bytes -= POLY1305_BLOCK_SIZE;
	}

	st->h[0] = h0 & 0xffffffff;
	st->h[1] = h1 & 0xffffffff;
	st->h[2] = h2 & 0xffffffff;
	st->h[3] = h3 & 0xffffffff;
	st->h[4] = h4 & 0xffffffff;
}


/* ----------------------------------------- */

void poly1305_init(poly1305_t *st, const void *param)
{
    const unsigned char *key = (const unsigned char *)param;

	/* r &= 0xffffffc0ffffffc0ffffffc0fffffff */

	st->r[0] = (get_le32(&key[ 0])     ) & 0x3ffffff;
	st->r[1] = (get_le32(&key[ 3]) >> 2) & 0x3ffff03;
	st->r[2] = (get_le32(&key[ 6]) >> 4) & 0x3ffc0ff;
	st->r[3] = (get_le32(&key[ 9]) >> 6) & 0x3f03fff;
	st->r[4] = (get_le32(&key[12]) >> 8) & 0x00fffff;

	/* h = 0 */

	st->h[0] = 0;
	st->h[1] = 0;
	st->h[2] = 0;
	st->h[3] = 0;
	st->h[4] = 0;

	/* save pad for later */

	st->pad[0] = get_le32(&key[16]);
	st->pad[1] = get_le32(&key[20]);
	st->pad[2] = get_le32(&key[24]);
	st->pad[3] = get_le32(&key[28]);

	st->leftover = 0;
	st->final = 0;
}

/* ----------------------------------------- */


void poly1305_update(poly1305_t *st, const void *buf, size_t bytes)
{
	size_t i,want;
    const unsigned char *m = (const unsigned char *)buf;

	/* handle leftover */

	if (st->leftover)
	{
		want = (POLY1305_BLOCK_SIZE - st->leftover);
		if (want > bytes)
			want = bytes;

		for (i = 0; i < want; i++)
			st->buffer[st->leftover + i] = m[i];

		bytes -= want;
		m += want;
		st->leftover += want;

		if (st->leftover < POLY1305_BLOCK_SIZE)
			return;

		poly1305_blocks(st, st->buffer, POLY1305_BLOCK_SIZE);

		st->leftover = 0;
	}

	/* process full blocks */

	if (bytes >= POLY1305_BLOCK_SIZE)
	{
		want = (bytes & ~(POLY1305_BLOCK_SIZE - 1));

		poly1305_blocks(st, m, want);

		m += want;
		bytes -= want;
	}

	/* store leftover */

	if (bytes)
	{
		for (i = 0; i < bytes; i++)
			st->buffer[st->leftover + i] = m[i];

	    st->leftover += bytes;
	}
}

/* ----------------------------------------- */


void poly1305_final(poly1305_t *st, void *hash)
{
	ul_t   h0,h1,h2,h3,h4,c,g0,g1,g2,g3,g4,mask;
	ull_t  f;
    unsigned char   *mac;

    mac = (unsigned char *)hash;

	/* process the remaining block */

	if (st->leftover)
	{
		size_t i = st->leftover;

		st->buffer[i++] = 1;

		for (; i < POLY1305_BLOCK_SIZE; i++)
			st->buffer[i] = 0;

		st->final = 1;

		poly1305_blocks(st, st->buffer, POLY1305_BLOCK_SIZE);
	}

	/* fully carry h */

	h0 = st->h[0];
	h1 = st->h[1];
	h2 = st->h[2];
	h3 = st->h[3];
	h4 = st->h[4];

	             c = h1 >> 26; h1 = h1 & 0x3ffffff;
	h2 +=     c; c = h2 >> 26; h2 = h2 & 0x3ffffff;
	h3 +=     c; c = h3 >> 26; h3 = h3 & 0x3ffffff;
	h4 +=     c; c = h4 >> 26; h4 = h4 & 0x3ffffff;
	h0 += c * 5; c = h0 >> 26; h0 = h0 & 0x3ffffff;
	h1 +=     c;

	/* compute h + -p */

	g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
	g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
	g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
	g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
	g4 = h4 + c - (1UL << 26);

	/* select h if h < p, or h + -p if h >= p */

	mask = (g4 >> ((sizeof(ul_t) * 8) - 1)) - 1;

	g0 &= mask;
	g1 &= mask;
	g2 &= mask;
	g3 &= mask;
	g4 &= mask;

	mask = ~mask;

	h0 = (h0 & mask) | g0;
	h1 = (h1 & mask) | g1;
	h2 = (h2 & mask) | g2;
	h3 = (h3 & mask) | g3;
	h4 = (h4 & mask) | g4;

	/* h = h % (2^128) */

	h0 = ((h0      ) | (h1 << 26)) & 0xffffffff;
	h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
	h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
	h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

	/* mac = (h + pad) % (2^128) */

	f = (ull_t) h0 + st->pad[0]            ; h0 = (ul_t)f;
	f = (ull_t) h1 + st->pad[1] + (f >> 32); h1 = (ul_t)f;
	f = (ull_t) h2 + st->pad[2] + (f >> 32); h2 = (ul_t)f;
	f = (ull_t) h3 + st->pad[3] + (f >> 32); h3 = (ul_t)f;

	put_le32(mac +  0, h0);
	put_le32(mac +  4, h1);
	put_le32(mac +  8, h2);
	put_le32(mac + 12, h3);

	/* zero out the state */

	st->h[0] = 0;
	st->h[1] = 0;
	st->h[2] = 0;
	st->h[3] = 0;
	st->h[4] = 0;
	st->r[0] = 0;
	st->r[1] = 0;
	st->r[2] = 0;
	st->r[3] = 0;
	st->r[4] = 0;
	st->pad[0] = 0;
	st->pad[1] = 0;
	st->pad[2] = 0;
	st->pad[3] = 0;
}

/* ----------------------------------------- */

void poly1305_partial(poly1305_t *st, void *hash)
{
    poly1305_t tmp;

    memcpy(&tmp,st,sizeof(tmp));
    poly1305_final(&tmp,hash);
}











