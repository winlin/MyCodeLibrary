/* MD5 routines, after Ron Rivest */
/* Written by David Madore <david.madore@ens.fr>, with code taken in
 * part from Colin Plumb. */
/* Public domain (1999/11/24) */

/* Note: these routines do not depend on endianness. */

/* === The header === */

/* Put this in md5.h if you don't like having everything in one big
 * file. */

#ifndef _DMADORE_MD5_H
#define _DMADORE_MD5_H

#include "md5func.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

struct md5_ctx {
    /* The four chaining variables */
    unsigned long buf[4];
    /* Count number of message bits */
    unsigned long bits[2];
    /* Data being fed in */
    unsigned long in[16];
    /* Our position within the 512 bits (always between 0 and 63) */
    int b;
};

void MD5_transform (unsigned long buf[4], const unsigned long in[16]);
void MD5_start (struct md5_ctx *context);
void MD5_feed (struct md5_ctx *context, unsigned char inb);
void MD5_stop (struct md5_ctx *context, unsigned char digest[16]);

#endif /* not defined _DMADORE_MD5_H */

/* === The implementation === */

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s) \
{ w += f (x, y, z) + data;  w = w<<s | (w&0xffffffffUL)>>(32-s); \
w += x; }

void
MD5_transform (unsigned long buf[4], const unsigned long in[16])
{
    register unsigned long a, b, c, d;
    
    a = buf[0];  b = buf[1];  c = buf[2];  d = buf[3];
    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478UL, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756UL, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070dbUL, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceeeUL, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0fafUL, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62aUL, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613UL, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501UL, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8UL, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7afUL, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1UL, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7beUL, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122UL, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193UL, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438eUL, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821UL, 22);
    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562UL, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340UL, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51UL, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aaUL, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105dUL, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453UL, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681UL, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8UL, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6UL, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6UL, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87UL, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14edUL, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905UL, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8UL, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9UL, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8aUL, 20);
    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942UL, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681UL, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122UL, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380cUL, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44UL, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9UL, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60UL, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70UL, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6UL, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127faUL, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085UL, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05UL, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039UL, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5UL, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8UL, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665UL, 23);
    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244UL, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97UL, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7UL, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039UL, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3UL, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92UL, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47dUL, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1UL, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4fUL, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0UL, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314UL, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1UL, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82UL, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235UL, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bbUL, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391UL, 21);
    buf[0] += a;  buf[1] += b;  buf[2] += c;  buf[3] += d;
}

#undef F1
#undef F2
#undef F3
#undef F4
#undef MD5STEP

void
MD5_start (struct md5_ctx *ctx)
{
    int i;
    
    ctx->buf[0] = 0x67452301UL;
    ctx->buf[1] = 0xefcdab89UL;
    ctx->buf[2] = 0x98badcfeUL;
    ctx->buf[3] = 0x10325476UL;
    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
    for ( i=0 ; i<16 ; i++ )
        ctx->in[i] = 0;
    ctx->b = 0;
}

void
MD5_feed (struct md5_ctx *ctx, unsigned char inb)
{
    int i;
    unsigned long temp;
    
    ctx->in[ctx->b/4] |= ((unsigned long)inb) << ((ctx->b%4)*8);
    if ( ++ctx->b >= 64 ) {
        MD5_transform (ctx->buf, ctx->in);
        ctx->b = 0;
        for ( i=0 ; i<16 ; i++ )
            ctx->in[i] = 0;
    }
    temp = ctx->bits[0];
    ctx->bits[0] += 8;
    if ( (temp&0xffffffffUL) > (ctx->bits[0]&0xffffffffUL) )
        ctx->bits[1]++;
}

void
MD5_stop (struct md5_ctx *ctx, unsigned char digest[16])
{
    int i;
    unsigned long bits[2];
    
    for ( i=0 ; i<2 ; i++ )
        bits[i] = ctx->bits[i];
    MD5_feed (ctx, 0x80);
    for ( ; ctx->b!=56 ; )
        MD5_feed (ctx, 0);
    for ( i=0 ; i<2 ; i++ ) {
        MD5_feed (ctx, bits[i]&0xff);
        MD5_feed (ctx, (bits[i]>>8)&0xff);
        MD5_feed (ctx, (bits[i]>>16)&0xff);
        MD5_feed (ctx, (bits[i]>>24)&0xff);
    }
    for ( i=0 ; i<4 ; i++ ) {
        digest[4*i] = ctx->buf[i]&0xff;
        digest[4*i+1] = (ctx->buf[i]>>8)&0xff;
        digest[4*i+2] = (ctx->buf[i]>>16)&0xff;
        digest[4*i+3] = (ctx->buf[i]>>24)&0xff;
    }
}

int md5_file(const char *file_path, char *md5str)
{
    int md5_char = 0;
    int j;
    struct md5_ctx context;
    unsigned char digest[16];
    FILE *f = fopen (file_path, "rb");
    if ( !f ) {
        fprintf (stderr, "Error opening %s\n", file_path);
    }
    
    MD5_start (&context);
    while ( 1 ) {
        int ch;
        
        ch = getc (f);
        if ( ch == EOF )
            break;
        MD5_feed (&context, ch);
        md5_char++;
    }
    MD5_stop (&context, digest);
    for ( j=0 ; j<16 ; j++ ) {
        sprintf(md5str+j*2, "%02x", digest[j]);
    }

    printf("md5_file   md5_char=%d\n", md5_char);
    fclose(f);
    return 0;
}

int md5_file2(const char *file_path, char *md5str)
{
    int md5_char = 0;
    int j;
    struct md5_ctx context;
    unsigned char digest[16];
    FILE *f = fopen (file_path, "rb");
    if ( !f ) {
        fprintf (stderr, "Error opening %s\n", file_path);
    }
    
    MD5_start (&context);

    unsigned char readstr[1024] = {0};
    int read_len = 0;
    while(feof(f) == 0) {
        read_len=fread(readstr, sizeof(unsigned char), 1024, f);
        for ( j=0; j<read_len; ++j) {
            MD5_feed(&context, readstr[j]);
            md5_char++;
        }
        memset(readstr, 0, read_len);    
    }
    MD5_stop (&context, digest);

    for ( j=0 ; j<16 ; j++ ) {
        sprintf(md5str+j*2, "%02x", digest[j]);
    }
    fclose(f);
    
    printf("md5_file2   md5_char=%d\n", md5_char);

    return 0;
}

long long int get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) return -1;
    return statbuf.st_size;
}

int md5_file3(const char *file_path, char *md5str)
{
    int md5_char = 0;
    int j;
    struct md5_ctx context;
    unsigned char digest[16];
    FILE *f = fopen (file_path, "rb");
    if ( !f ) {
        fprintf (stderr, "Error opening %s\n", file_path);
    }
    
    MD5_start (&context);


    long long int file_size = get_size_by_fd(fileno(f));
    if (file_size < 0) {
     fclose(f);
     return -1;
    }

    unsigned char *file_buffer = mmap(NULL, file_size, PROT_READ, MAP_SHARED|MAP_FILE, fileno(f), 0);
    if (file_buffer == MAP_FAILED) {
     fclose(f);
     perror("mmap() failed");
     return errno;
    }
    for (j = 0; j < file_size; ++j) {
        MD5_feed(&context, file_buffer[j]);
        md5_char++;
    }
    
    MD5_stop (&context, digest);
    for ( j=0 ; j<16 ; j++ ) {
        sprintf(md5str+j*2, "%02x", digest[j]);
    }

    munmap(file_buffer, file_size);
    fclose(f);

    printf("md5_file2   md5_char=%d\n", md5_char);

    return 0;
}
