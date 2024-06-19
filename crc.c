#include "crc.h"
#include <stddef.h>

// #define CRCPOLY 0x82f63b78 // reversed 0x1EDC6F41
#define CRCPOLY 0xEDB88320 // revered 0x04c11db7
#define CRCINIT 0xFFFFFFFF

uint32_t g_crc_slicing[8][256];


inline int Min(int x, int y)
{
    return y ^ ((x ^ y) & -(x < y));
}

static void _initCRC()
{
    unsigned int i, j, x, c;

    for (i = 0; i <= 0xFF; i++)
    {
        x = i;
        for (j = 0; j < 8; j++)
            x = (x>>1) ^ (CRCPOLY & (-(int)(x & 1)));
        g_crc_slicing[0][i] = x;
    }

    for (i = 0; i <= 0xFF; i++)
    {
        c = g_crc_slicing[0][i];
        for (j = 1; j < 8; j++)
        {
            c = g_crc_slicing[0][c & 0xFF] ^ (c >> 8);
            g_crc_slicing[j][i] = c;
        }
    }
}

uint32_t crc32_begin()
{
    static int initialized = 0;

    if (!initialized)
    {
        _initCRC();
        initialized = 1;
    }

    return CRCINIT;
}

uint32_t crc32_end(uint32_t crc)
{
    return ~crc;
}

/* Slicing by 8 algorithm */
uint32_t crc32_data(uint32_t crc, const char *buf, int len)
{
//        RES crc = CRCINIT;

        // Align to boundary
        int align = (sizeof(size_t *) - (size_t) buf) & (sizeof(size_t *) - 1);
        align = Min(align, len);
        len -= align;
        for (; align; align--)
                crc = g_crc_slicing[0][(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

        int nqwords = len / (sizeof(uint32_t) + sizeof(uint32_t));
        for (; nqwords; nqwords--) {
                crc ^= *(uint32_t*)buf;
                buf += sizeof(uint32_t);
                uint32_t next = *(uint32_t*)buf;
                buf += sizeof(uint32_t);
                crc =
                        g_crc_slicing[7][(crc      ) & 0xFF] ^
                        g_crc_slicing[6][(crc >>  8) & 0xFF] ^
                        g_crc_slicing[5][(crc >> 16) & 0xFF] ^
                        g_crc_slicing[4][(crc >> 24)] ^
                        g_crc_slicing[3][(next      ) & 0xFF] ^
                        g_crc_slicing[2][(next >>  8) & 0xFF] ^
                        g_crc_slicing[1][(next >> 16) & 0xFF] ^
                        g_crc_slicing[0][(next >> 24)];
        }

        len &= sizeof(uint32_t) * 2 - 1;
        for (; len; len--)
                crc = g_crc_slicing[0][(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

        return crc;
//        return ~crc;
}

