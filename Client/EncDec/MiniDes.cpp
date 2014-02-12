//

//
#include "MiniDes.h"

//
#include "sk.h"

//

#define DES_ENCRYPT	1
#define DES_DECRYPT	0

#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 16
#define HALF_ITERATIONS 8

#define c2l(c,l)	(l =((DES_LONG)(*((c)++)))    , \
			 l|=((DES_LONG)(*((c)++)))<< 8L, \
			 l|=((DES_LONG)(*((c)++)))<<16L, \
			 l|=((DES_LONG)(*((c)++)))<<24L)

/* NOTE - c is not incremented as per c2l */
#define c2ln(c,l1,l2,n)	{ \
			c+=n; \
			l1=l2=0; \
			switch (n) { \
			case 8: l2 =((DES_LONG)(*(--(c))))<<24L; \
			case 7: l2|=((DES_LONG)(*(--(c))))<<16L; \
			case 6: l2|=((DES_LONG)(*(--(c))))<< 8L; \
			case 5: l2|=((DES_LONG)(*(--(c))));     \
			case 4: l1 =((DES_LONG)(*(--(c))))<<24L; \
			case 3: l1|=((DES_LONG)(*(--(c))))<<16L; \
			case 2: l1|=((DES_LONG)(*(--(c))))<< 8L; \
			case 1: l1|=((DES_LONG)(*(--(c))));     \
				} \
			}

#define l2c(l,c)	(*((c)++)=(unsigned char)(((l)     )&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>24L)&0xff))

/* replacements for htonl and ntohl since I have no idea what to do
 * when faced with machines with 8 byte longs. */
#define HDRSIZE 4

#define n2l(c,l)	(l =((DES_LONG)(*((c)++)))<<24L, \
			 l|=((DES_LONG)(*((c)++)))<<16L, \
			 l|=((DES_LONG)(*((c)++)))<< 8L, \
			 l|=((DES_LONG)(*((c)++))))

#define l2n(l,c)	(*((c)++)=(unsigned char)(((l)>>24L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
			 *((c)++)=(unsigned char)(((l)     )&0xff))

/* NOTE - c is not incremented as per l2c */
#define l2cn(l1,l2,c,n)	{ \
			c+=n; \
			switch (n) { \
			case 8: *(--(c))=(unsigned char)(((l2)>>24L)&0xff); \
			case 7: *(--(c))=(unsigned char)(((l2)>>16L)&0xff); \
			case 6: *(--(c))=(unsigned char)(((l2)>> 8L)&0xff); \
			case 5: *(--(c))=(unsigned char)(((l2)     )&0xff); \
			case 4: *(--(c))=(unsigned char)(((l1)>>24L)&0xff); \
			case 3: *(--(c))=(unsigned char)(((l1)>>16L)&0xff); \
			case 2: *(--(c))=(unsigned char)(((l1)>> 8L)&0xff); \
			case 1: *(--(c))=(unsigned char)(((l1)     )&0xff); \
				} \
			}

#define	ROTATE(a,n)	(_lrotr(a,n))

#define D_ENCRYPT(LL,R,S) {\
	u=(R^s[S  ]); \
	t=R^s[S+1]; \
	t=ROTATE(t,4); \
	LL^=\
		desSPtrans[0][(u>> 2L)&0x3f]^ \
		desSPtrans[2][(u>>10L)&0x3f]^ \
		desSPtrans[4][(u>>18L)&0x3f]^ \
		desSPtrans[6][(u>>26L)&0x3f]^ \
		desSPtrans[1][(t>> 2L)&0x3f]^ \
		desSPtrans[3][(t>>10L)&0x3f]^ \
		desSPtrans[5][(t>>18L)&0x3f]^ \
		desSPtrans[7][(t>>26L)&0x3f]; }

	/* IP and FP
	 * The problem is more of a geometric problem that random bit fiddling.
	 0  1  2  3  4  5  6  7      62 54 46 38 30 22 14  6
	 8  9 10 11 12 13 14 15      60 52 44 36 28 20 12  4
	16 17 18 19 20 21 22 23      58 50 42 34 26 18 10  2
	24 25 26 27 28 29 30 31  to  56 48 40 32 24 16  8  0

	32 33 34 35 36 37 38 39      63 55 47 39 31 23 15  7
	40 41 42 43 44 45 46 47      61 53 45 37 29 21 13  5
	48 49 50 51 52 53 54 55      59 51 43 35 27 19 11  3
	56 57 58 59 60 61 62 63      57 49 41 33 25 17  9  1

	The output has been subject to swaps of the form
	0 1 -> 3 1 but the odd and even bits have been put into
	2 3    2 0
	different words.  The main trick is to remember that
	t=((l>>size)^r)&(mask);
	r^=t;
	l^=(t<<size);
	can be used to swap and move bits between words.

	So l =  0  1  2  3  r = 16 17 18 19
	        4  5  6  7      20 21 22 23
	        8  9 10 11      24 25 26 27
	       12 13 14 15      28 29 30 31
	becomes (for size == 2 and mask == 0x3333)
	   t =   2^16  3^17 -- --   l =  0  1 16 17  r =  2  3 18 19
		 6^20  7^21 -- --        4  5 20 21       6  7 22 23
		10^24 11^25 -- --        8  9 24 25      10 11 24 25
		14^28 15^29 -- --       12 13 28 29      14 15 28 29

	Thanks for hints from Richard Outerbridge - he told me IP&FP
	could be done in 15 xor, 10 shifts and 5 ands.
	When I finally started to think of the problem in 2D
	I first got ~42 operations without xors.  When I remembered
	how to use xors :-) I got it to its final state.
	*/

#define IP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(r,l,tt, 4,0x0f0f0f0fL); \
	PERM_OP(l,r,tt,16,0x0000ffffL); \
	PERM_OP(r,l,tt, 2,0x33333333L); \
	PERM_OP(l,r,tt, 8,0x00ff00ffL); \
	PERM_OP(r,l,tt, 1,0x55555555L); \
	}

#define FP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(l,r,tt, 1,0x55555555L); \
	PERM_OP(r,l,tt, 8,0x00ff00ffL); \
	PERM_OP(l,r,tt, 2,0x33333333L); \
	PERM_OP(r,l,tt,16,0x0000ffffL); \
	PERM_OP(l,r,tt, 4,0x0f0f0f0fL); \
	}

#define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
	(b)^=(t),\
	(a)^=((t)<<(n)))

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
	(a)=(a)^(t)^(t>>(16-(n))))\

int DesSetKey(DesCBlock* key, DesKeySchedule schedule)
{
	static int shifts2[16]={0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};
	register DES_LONG c,d,t,s,t2;
	register unsigned char *in;
	register DES_LONG *k;
	register int i;

	k=(DES_LONG *)schedule;
	in=(unsigned char *)key;

	c2l(in,c);
	c2l(in,d);

	/* I now do it in 47 simple operations :-)
	* Thanks to John Fletcher (john_fletcher@lccmail.ocf.llnl.gov)
	* for the inspiration. :-) */
	PERM_OP (d,c,t,4,0x0f0f0f0fL);
	HPERM_OP(c,t,-2,0xcccc0000L);
	HPERM_OP(d,t,-2,0xcccc0000L);
	PERM_OP (d,c,t,1,0x55555555L);
	PERM_OP (c,d,t,8,0x00ff00ffL);
	PERM_OP (d,c,t,1,0x55555555L);
	d=	(((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
		((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
	c&=0x0fffffffL;

	for (i=0; i<ITERATIONS; i++)
	{
		if (shifts2[i])
		{ c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
		else
		{ c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
		c&=0x0fffffffL;
		d&=0x0fffffffL;
		/* could be a few less shifts but I am to lazy at this
		* point in time to investigate */
		s=	des_skb[0][ (c    )&0x3f                ]|
			des_skb[1][((c>> 6)&0x03)|((c>> 7L)&0x3c)]|
			des_skb[2][((c>>13)&0x0f)|((c>>14L)&0x30)]|
			des_skb[3][((c>>20)&0x01)|((c>>21L)&0x06) |
			((c>>22L)&0x38)];
		t=	des_skb[4][ (d    )&0x3f                ]|
			des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
			des_skb[6][ (d>>15L)&0x3f                ]|
			des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

		/* table contained 0213 4657 */
		t2=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
		*(k++)=((t2<<2L)|(t2>>30L))&0xffffffffL;

		t2=((s>>16L)|(t&0xffff0000L));
		*(k++)=((t2<<6L)|(t2>>26L))&0xffffffffL;
	}
	return 0;
}

void DesEncrypt(DES_LONG *data, DesKeySchedule ks, DesSPtrans desSPtrans, int enc)
{
	register DES_LONG l,r,t,u;

	register int i;

	register DES_LONG *s;

	r=data[0];
	l=data[1];

	IP(r,l);
	/* Things have been modified so that the initial rotate is
	* done outside the loop.  This required the
	* des_SPtrans values in sp.h to be rotated 1 bit to the right.
	* One perl script later and things have a 5% speed up on a sparc2.
	* Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
	* for pointing this out. */
	/* clear the top bits on machines with 8byte longs */
	/* shift left by 2 */
	r=ROTATE(r,29)&0xffffffffL;
	l=ROTATE(l,29)&0xffffffffL;

	s=(DES_LONG *)ks;
	/* I don't know if it is worth the effort of loop unrolling the
	* inner loop */
	if (enc)
	{
		for (i=0; i<32; i+=8)
		{
			D_ENCRYPT(l,r,i+0); /*  1 */
			D_ENCRYPT(r,l,i+2); /*  2 */
			D_ENCRYPT(l,r,i+4); /*  3 */
			D_ENCRYPT(r,l,i+6); /*  4 */
		}
	}
	else
	{
		for (i=30; i>0; i-=8)
		{
			D_ENCRYPT(l,r,i-0); /* 16 */
			D_ENCRYPT(r,l,i-2); /* 15 */
			D_ENCRYPT(l,r,i-4); /* 14 */
			D_ENCRYPT(r,l,i-6); /* 13 */
		}
	}

	/* rotate and clear the top bits on machines with 8byte longs */
	l=ROTATE(l,3)&0xffffffffL;
	r=ROTATE(r,3)&0xffffffffL;

	FP(r,l);
	data[0]=l;
	data[1]=r;
	l=r=t=u=0;
}

void DesEncrypt2(DES_LONG *data, DesKeySchedule ks, DesSPtrans desSPtrans, int enc)
{
	register DES_LONG l,r,t,u;
	register int i;
	register DES_LONG *s;

	r=data[0];
	l=data[1];

	/* Things have been modified so that the initial rotate is
	* done outside the loop.  This required the
	* des_SPtrans values in sp.h to be rotated 1 bit to the right.
	* One perl script later and things have a 5% speed up on a sparc2.
	* Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
	* for pointing this out. */
	/* clear the top bits on machines with 8byte longs */
	r=ROTATE(r,29)&0xffffffff;
	l=ROTATE(l,29)&0xffffffff;

	s=(DES_LONG *)ks;
	/* I don't know if it is worth the effort of loop unrolling the
	* inner loop */
	if (enc)
	{
		for (i=0; i<32; i+=8)
		{
			D_ENCRYPT(l,r,i+0); /*  1 */
			D_ENCRYPT(r,l,i+2); /*  2 */
			D_ENCRYPT(l,r,i+4); /*  3 */
			D_ENCRYPT(r,l,i+6); /*  4 */
		}
	}
	else
	{
		for (i=30; i>0; i-=8)
		{
			D_ENCRYPT(l,r,i-0); /* 16 */
			D_ENCRYPT(r,l,i-2); /* 15 */
			D_ENCRYPT(l,r,i-4); /* 14 */
			D_ENCRYPT(r,l,i-6); /* 13 */
		}
	}
	/* rotate and clear the top bits on machines with 8byte longs */
	data[0]=ROTATE(l,3)&0xffffffff;
	data[1]=ROTATE(r,3)&0xffffffff;
	l=r=t=u=0;
}

void DesEncrypt3
(
 DES_LONG* data,
 DesKeySchedule ks1,
 DesKeySchedule ks2,
 DesKeySchedule ks3, 
 DesSPtrans desSPtrans
 )
{
	register DES_LONG l,r;

	l=data[0];
	r=data[1];
	IP(l,r);
	data[0]=l;
	data[1]=r;
	DesEncrypt2((DES_LONG *)data, ks1, desSPtrans, DES_ENCRYPT);
	DesEncrypt2((DES_LONG *)data, ks2, desSPtrans, DES_DECRYPT);
	DesEncrypt2((DES_LONG *)data, ks3, desSPtrans, DES_ENCRYPT);
	l=data[0];
	r=data[1];
	FP(r,l);
	data[0]=l;
	data[1]=r;
}

void DesDecrypt3
(
 DES_LONG* data, 
 DesKeySchedule ks1,
 DesKeySchedule ks2, 
 DesKeySchedule ks3, 
 DesSPtrans desSPtrans
 )
{
	register DES_LONG l,r;

	l=data[0];
	r=data[1];
	IP(l,r);
	data[0]=l;
	data[1]=r;
	DesEncrypt2((DES_LONG *)data, ks3, desSPtrans, DES_DECRYPT);
	DesEncrypt2((DES_LONG *)data, ks2, desSPtrans, DES_ENCRYPT);
	DesEncrypt2((DES_LONG *)data, ks1, desSPtrans, DES_DECRYPT);
	l=data[0];
	r=data[1];
	FP(r,l);
	data[0]=l;
	data[1]=r;
}