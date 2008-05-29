/* This file comes from linux kernel sources */
/*
 * bitmaps provide bit arrays that consume one or more unsigned
 * longs.  The bitmap interface and available operations are listed
 * here, in bitmap.h
 *
 * See bitmap.c for more details.
 */

/*
 * The available bitmap operations and their rough meaning in the
 * case that the bitmap is a single unsigned long are thus:
 *
 * Note that nbits should be always a compile time evaluable constant.
 * Otherwise many inlines will generate horrible code.
 *
 * bitmap_zero(dst, nbits)			*dst = 0UL
 * bitmap_fill(dst, nbits)			*dst = ~0UL
 * bitmap_copy(dst, src, nbits)			*dst = *src
 * bitmap_and(dst, src1, src2, nbits)		*dst = *src1 & *src2
 * bitmap_or(dst, src1, src2, nbits)		*dst = *src1 | *src2
 * bitmap_xor(dst, src1, src2, nbits)		*dst = *src1 ^ *src2
 * bitmap_andnot(dst, src1, src2, nbits)	*dst = *src1 & ~(*src2)
 * bitmap_complement(dst, src, nbits)		*dst = ~(*src)
 * bitmap_equal(src1, src2, nbits)		Are *src1 and *src2 equal?
 * bitmap_intersects(src1, src2, nbits) 	Do *src1 and *src2 overlap?
 * bitmap_subset(src1, src2, nbits)		Is *src1 a subset of *src2?
 * bitmap_empty(src, nbits)			Are all bits zero in *src?
 * bitmap_full(src, nbits)			Are all bits set in *src?
 * bitmap_weight(src, nbits)			Hamming Weight: number set bits
 * bitmap_shift_right(dst, src, n, nbits)	*dst = *src >> n
 * bitmap_shift_left(dst, src, n, nbits)	*dst = *src << n
 * bitmap_remap(dst, src, old, new, nbits)	*dst = map(old, new)(src)
 * bitmap_bitremap(oldbit, old, new, nbits)	newbit = map(old, new)(oldbit)
 * bitmap_scnprintf(buf, len, src, nbits)	Print bitmap src to buf
 * bitmap_parse(buf, buflen, dst, nbits)	Parse bitmap dst from kernel buf
 * bitmap_parse_user(ubuf, ulen, dst, nbits)	Parse bitmap dst from user buf
 * bitmap_scnlistprintf(buf, len, src, nbits)	Print bitmap src as list to buf
 * bitmap_scneachprintf(buf, len, src, nbits)	Print each bit in bitmap src to buf
 * bitmap_parselist(buf, dst, nbits)		Parse bitmap dst from list
 * bitmap_find_free_region(bitmap, bits, order)	Find and allocate bit region
 * bitmap_release_region(bitmap, pos, order)	Free specified bit region
 * bitmap_allocate_region(bitmap, pos, order)	Allocate specified bit region
 *
 * set_bit(bit, addr)			*addr |= bit
 * clear_bit(bit, addr)			*addr &= ~bit
 * change_bit(bit, addr)		*addr ^= bit
 * test_bit(bit, addr)			Is bit set in *addr?
 * test_and_set_bit(bit, addr)		Set bit and return old value
 * test_and_clear_bit(bit, addr)	Clear bit and return old value
 * test_and_change_bit(bit, addr)	Change bit and return old value
 * find_first_zero_bit(addr, nbits)	Position first zero bit in *addr
 * find_first_bit(addr, nbits)		Position first set bit in *addr
 * find_next_zero_bit(addr, nbits, bit)	Position next zero bit in *addr >= bit
 * find_next_bit(addr, nbits, bit)	Position next set bit in *addr >= bit
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#define BITS_PER_LONG __WORDSIZE
#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#define BITMAP_LAST_WORD_MASK(nbits)					\
(									\
	((nbits) % BITS_PER_LONG) ?					\
		(1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL		\
)

#define BITS_TO_LONGS(n) ((n + BITS_PER_LONG - 1) / BITS_PER_LONG)

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x < _y ? _x : _y; })

#define max(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x > _y ? _x : _y; })

#if __WORDSIZE == 64
typedef uint64_t T;
enum {
	BITMAP_SHIFT   = 6,
	BITMAP_MASK    = 63,
};
#else
typedef uint32_t T;
enum {
	BITMAP_SHIFT  = 5,
	BITMAP_MASK   = 31
};
#endif

typedef uint32_t u32;
typedef uint64_t __u64;

unsigned int hweight32(unsigned int w);
unsigned int hweight16(unsigned int w);
unsigned int hweight8(unsigned int w);
unsigned long hweight64(__u64 w);

static inline unsigned long hweight_long(unsigned long w)
{
        return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

/*
 * Dumb version
 */
static inline unsigned long __ffs(unsigned long b)
{
	int i;
	for (i=0; i < BITS_PER_LONG; i++)
		if (b & (1UL << i))
			return i;
	return BITS_PER_LONG;
}

/*
 * Dumb version
 */
static inline unsigned long __fls(unsigned long b)
{
	int i;
	for (i=BITS_PER_LONG-1; i >= 0; i--)
		if (b & (1UL << i))
			return i;
	return BITS_PER_LONG;
}

static inline void set_bit(unsigned int offset, unsigned long *chunk)
{
	unsigned int i = offset >> BITMAP_SHIFT;
	chunk[i] |= (1UL << (offset & BITMAP_MASK));
}

static inline int test_bit(unsigned int offset, const unsigned long *chunk)
{
	unsigned int i = offset >> BITMAP_SHIFT;
	return (chunk[i] & (1UL << (offset & BITMAP_MASK))) != 0;
}

/**
 * Find first non-zero bit starting at offset
 * @param offset bit offset to start with
 * @return index of the non-zero bit or -1 if not found.
 */
static inline unsigned long find_next_bit(const unsigned long *chunk, unsigned int nbits, unsigned int offset)
{
	unsigned int nchunks = BITS_TO_LONGS(nbits);
	unsigned int i = offset >> BITMAP_SHIFT;

	// Check the first chunk.
	// Mask out bits before the start bit.
	unsigned long d = chunk[i] & (~0UL << (offset & BITMAP_MASK));
	if (d)
		return __ffs(d) + (i << BITMAP_SHIFT);

	// Scan the rest of the chunks
	for (++i; i < nchunks; i++)
		if (chunk[i])
			return __ffs(chunk[i]) + (i << BITMAP_SHIFT);
	return nbits;
}

#define find_first_bit(b, n) find_next_bit(b, n, 0)

/**
 * Find last non-zero bit.
 * @return index of the non-zero bit or -1 if not found.
 */
static inline unsigned long find_last(const unsigned long *chunk, unsigned int nbits)
{
	unsigned int nchunks = BITS_TO_LONGS(nbits);
	int i;

	// Scan the rest of the chunks
	for (i = nchunks - 1; i >=0; i--)
		if (chunk[i])
			return __fls(chunk[i]) + (i << BITMAP_SHIFT);
	return nbits;
}
/*
 * bitmap.c provides these functions:
 */
extern int __bitmap_empty(const unsigned long *bitmap, int bits);
extern int __bitmap_full(const unsigned long *bitmap, int bits);
extern int __bitmap_equal(const unsigned long *bitmap1,
                	const unsigned long *bitmap2, int bits);
extern void __bitmap_complement(unsigned long *dst, const unsigned long *src,
			int bits);
extern void __bitmap_shift_right(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern void __bitmap_shift_left(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern void __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern void __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern int __bitmap_intersects(const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern int __bitmap_subset(const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits);
extern int __bitmap_weight(const unsigned long *bitmap, int bits);

extern int bitmap_scnprintf(char *buf, unsigned int len,
			const unsigned long *src, int nbits);
extern int __bitmap_parse(const char *buf, unsigned int buflen, int is_user,
			unsigned long *dst, int nbits);
extern int bitmap_scnlistprintf(char *buf, unsigned int len,
			const unsigned long *src, int nbits);
extern int bitmap_scneachprintf(char *buf, unsigned int len,
			const unsigned long *src, int nbits);
extern int bitmap_parselist(const char *buf, unsigned long *maskp,
			int nmaskbits);
extern void bitmap_remap(unsigned long *dst, const unsigned long *src,
		const unsigned long *old, const unsigned long *new, int bits);
extern int bitmap_bitremap(int oldbit,
		const unsigned long *old, const unsigned long *new, int bits);
extern int bitmap_find_free_region(unsigned long *bitmap, int bits, int order);
extern void bitmap_release_region(unsigned long *bitmap, int pos, int order);
extern int bitmap_allocate_region(unsigned long *bitmap, int pos, int order);

static inline void bitmap_zero(unsigned long *dst, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = 0UL;
	else {
		int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memset(dst, 0, len);
	}
}

static inline void bitmap_fill(unsigned long *dst, int nbits)
{
	size_t nlongs = BITS_TO_LONGS(nbits);
	if (nlongs > 1) {
		int len = (nlongs - 1) * sizeof(unsigned long);
		memset(dst, 0xff,  len);
	}
	dst[nlongs - 1] = BITMAP_LAST_WORD_MASK(nbits);
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src,
			int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src;
	else {
		int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memcpy(dst, src, len);
	}
}

static inline void bitmap_and(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 & *src2;
	else
		__bitmap_and(dst, src1, src2, nbits);
}

static inline void bitmap_or(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 | *src2;
	else
		__bitmap_or(dst, src1, src2, nbits);
}

static inline void bitmap_xor(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 ^ *src2;
	else
		__bitmap_xor(dst, src1, src2, nbits);
}

static inline void bitmap_andnot(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 & ~(*src2);
	else
		__bitmap_andnot(dst, src1, src2, nbits);
}

static inline void bitmap_complement(unsigned long *dst, const unsigned long *src,
			int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = ~(*src) & BITMAP_LAST_WORD_MASK(nbits);
	else
		__bitmap_complement(dst, src, nbits);
}

static inline int bitmap_equal(const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ! ((*src1 ^ *src2) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_equal(src1, src2, nbits);
}

static inline int bitmap_intersects(const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ((*src1 & *src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;
	else
		return __bitmap_intersects(src1, src2, nbits);
}

static inline int bitmap_subset(const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ! ((*src1 & ~(*src2)) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_subset(src1, src2, nbits);
}

static inline int bitmap_empty(const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ! (*src & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_empty(src, nbits);
}

static inline int bitmap_full(const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ! (~(*src) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_full(src, nbits);
}

static inline int bitmap_weight(const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return hweight_long(*src & BITMAP_LAST_WORD_MASK(nbits));
	return __bitmap_weight(src, nbits);
}

static inline void bitmap_shift_right(unsigned long *dst,
			const unsigned long *src, int n, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src >> n;
	else
		__bitmap_shift_right(dst, src, n, nbits);
}

static inline void bitmap_shift_left(unsigned long *dst,
			const unsigned long *src, int n, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = (*src << n) & BITMAP_LAST_WORD_MASK(nbits);
	else
		__bitmap_shift_left(dst, src, n, nbits);
}

static inline int bitmap_parse(const char *buf, unsigned int buflen,
			unsigned long *maskp, int nmaskbits)
{
	return __bitmap_parse(buf, buflen, 0, maskp, nmaskbits);
}

#endif /* BITMAP_H */
