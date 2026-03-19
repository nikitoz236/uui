#include "utf.h"

unsigned utf16_to_utf8(uint8_t * dst, unsigned dst_max, const uint16_t * src, unsigned src_len)
{
    unsigned si = 0;
    unsigned di = 0;
    while (1) {
        uint16_t ch;
        if (src_len != 0) {
            if (si >= src_len) {
                break;
            }
        }
        if (dst_max == 0) {
            break;
        }
        ch = src[si++];

        if (ch == 0) {
            break;
        }

        if (ch >= 0xD800 && ch <= 0xDFFF) {
            dst[di++] = '@';
            si++;
        } else {
            if (ch <= 0x7F) {
                dst[di++] = (uint8_t)ch;
            } else if (ch <= 0x7FF) {
                if (dst_max < 2) {
                    break;
                }
                dst[di++] = (uint8_t)(0xC0 | ((ch >> 6) & 0x1F));
                dst[di++] = (uint8_t)(0x80 | (ch & 0x3F));
                dst_max--;
            } else {
                if (dst_max < 3) {
                    break;
                }
                dst[di++] = (uint8_t)(0xE0 | ((ch >> 12) & 0x0F));
                dst[di++] = (uint8_t)(0x80 | ((ch >> 6) & 0x3F));
                dst[di++] = (uint8_t)(0x80 | (ch & 0x3F));
                dst_max -= 2;
            }
        }
        dst_max--;
    }
    return di;
}