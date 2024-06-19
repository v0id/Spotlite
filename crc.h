#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t crc32_begin();
    uint32_t crc32_data(uint32_t crc, const char *buf, int len);
    uint32_t crc32_end(uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif
